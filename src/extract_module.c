#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <yaml.h>

#include "module_config.pb-c.h"

int initialize_parser(const char *filename, yaml_parser_t *parser, FILE **fh)
{
    *fh = fopen(filename, "r");

    /* Initialize parser */
    if (!yaml_parser_initialize(parser))
    {
        fprintf(stderr, "Error: Failed to initialize parser!\n");
        return -1;
    }
    if (*fh == NULL)
    {
        fprintf(stderr, "Error: Failed to open file %s!\n", filename);
        yaml_parser_delete(parser);
        return -1;
    }

    /* Set input file */
    yaml_parser_set_input_file(parser, *fh);

    return 0;
}

void cleanup_resources(yaml_parser_t *parser, yaml_event_t *event, FILE *fh)
{
    if (event != NULL)
    {
        yaml_event_delete(event);
    }
    yaml_parser_delete(parser);

    if (fh != NULL)
    {
        fclose(fh);
    }
}

enum state
{
    STATE_START,
    STATE_STREAM,
    STATE_DOCUMENT,
    STATE_SECTION,         /* top-level mapping */
    STATE_LATENCY,         /* reading latency_cost value */
    STATE_ENERGY,          /* reading energy_cost value */
    STATE_PARAMETERS_LIST, /* sequence of parameter mappings */
    STATE_PARAM_MAPPING,   /* inside a parameter mapping */
    STATE_PARAM_KEY,       /* expecting field name inside parameter */
    STATE_PARAM_KEYNAME,   /* reading the 'key' scalar value */
    STATE_PARAM_TYPE,      /* reading the 'type' scalar value */
    STATE_PARAM_VALUE,     /* reading the 'value' scalar value */
    STATE_STOP
};

struct parser_state
{
    enum state state;
    ModuleConfig m;
    /* temp parameter being built */
    char *p_key;
    ConfigParameter__ValueCase p_value_case;
    union
    {
        char *string_value;
        float float_value;
        int32_t int_value;
        protobuf_c_boolean bool_value;
    } p_value;
    /* storage of allocated parameter pointers */
    ConfigParameter **plist;
    size_t n_parameters;
};

static void reset_current_param(struct parser_state *s)
{
    if (s->p_key)
    {
        free(s->p_key);
        s->p_key = NULL;
    }
    s->p_value_case = CONFIG_PARAMETER__VALUE__NOT_SET;
    s->p_value.string_value = NULL;
}

/* Consume YAML events and update parser_state */
int consume_event(struct parser_state *s, yaml_event_t *event)
{
    char *value;

    switch (s->state)
    {
    case STATE_START:
        if (event->type == YAML_STREAM_START_EVENT)
            s->state = STATE_STREAM;
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_START\n", event->type);
            return -1;
        }
        break;

    case STATE_STREAM:
        if (event->type == YAML_DOCUMENT_START_EVENT)
            s->state = STATE_DOCUMENT;
        else if (event->type == YAML_STREAM_END_EVENT)
            s->state = STATE_STOP;
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_STREAM\n", event->type);
            return -1;
        }
        break;

    case STATE_DOCUMENT:
        if (event->type == YAML_MAPPING_START_EVENT)
            s->state = STATE_SECTION;
        else if (event->type == YAML_DOCUMENT_END_EVENT)
            s->state = STATE_STREAM;
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_DOCUMENT\n", event->type);
            return -1;
        }
        break;

    case STATE_SECTION:
        if (event->type == YAML_SCALAR_EVENT)
        {
            value = (char *)event->data.scalar.value;
            if (strcmp(value, "latency_cost") == 0)
                s->state = STATE_LATENCY;
            else if (strcmp(value, "energy_cost") == 0)
                s->state = STATE_ENERGY;
            else if (strcmp(value, "parameters") == 0)
                s->state = STATE_PARAMETERS_LIST;
            else
            {
                fprintf(stderr, "Unexpected top-level key: %s\n", value);
                return -1;
            }
        }
        else if (event->type == YAML_MAPPING_END_EVENT)
        {
            /* end of document mapping */
            s->state = STATE_DOCUMENT;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_SECTION\n", event->type);
            return -1;
        }
        break;

    case STATE_LATENCY:
        if (event->type == YAML_SCALAR_EVENT)
        {
            s->m.latency_cost = atoi((char *)event->data.scalar.value);
            s->state = STATE_SECTION;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d reading latency_cost\n", event->type);
            return -1;
        }
        break;

    case STATE_ENERGY:
        if (event->type == YAML_SCALAR_EVENT)
        {
            s->m.energy_cost = atoi((char *)event->data.scalar.value);
            s->state = STATE_SECTION;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d reading energy_cost\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAMETERS_LIST:
        if (event->type == YAML_SEQUENCE_START_EVENT)
            s->state = STATE_PARAM_MAPPING;
        else if (event->type == YAML_SCALAR_EVENT)
        {
            /* tolerate accidental scalar, but treat as error */
            fprintf(stderr, "Unexpected scalar in parameters list: %s\n", (char *)event->data.scalar.value);
            return -1;
        }
        else if (event->type == YAML_SEQUENCE_END_EVENT)
        {
            /* empty list */
            s->state = STATE_SECTION;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_PARAMETERS_LIST\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAM_MAPPING:
        if (event->type == YAML_MAPPING_START_EVENT)
        {
            reset_current_param(s);
            s->state = STATE_PARAM_KEY;
        }
        else if (event->type == YAML_SEQUENCE_END_EVENT)
        {
            /* finished sequence of parameters */
            s->state = STATE_SECTION;
            /* assign collected list to ModuleConfig */
            s->m.n_parameters = s->n_parameters;
            s->m.parameters = s->plist;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_PARAM_MAPPING\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAM_KEY:
        if (event->type == YAML_SCALAR_EVENT)
        {
            value = (char *)event->data.scalar.value;
            if (strcmp(value, "key") == 0)
                s->state = STATE_PARAM_KEYNAME;
            else if (strcmp(value, "type") == 0)
                s->state = STATE_PARAM_TYPE;
            else if (strcmp(value, "value") == 0)
                s->state = STATE_PARAM_VALUE;
            else
            {
                fprintf(stderr, "Unexpected parameter key: %s\n", value);
                return -1;
            }
        }
        else if (event->type == YAML_MAPPING_END_EVENT)
        {
            /* end of one parameter mapping: allocate and push */
            ConfigParameter *p = malloc(sizeof(ConfigParameter));
            if (!p)
            {
                fprintf(stderr, "Memory allocation failed for ConfigParameter\n");
                return -1;
            }
            /* initialize descriptor base using provided macro if necessary */
            *p = (ConfigParameter)CONFIG_PARAMETER__INIT;
            p->key = s->p_key ? strdup(s->p_key) : strdup("");
            p->value_case = s->p_value_case;
            switch (s->p_value_case)
            {
            case CONFIG_PARAMETER__VALUE_BOOL_VALUE:
                p->bool_value = s->p_value.bool_value;
                break;
            case CONFIG_PARAMETER__VALUE_INT_VALUE:
                p->int_value = s->p_value.int_value;
                break;
            case CONFIG_PARAMETER__VALUE_FLOAT_VALUE:
                p->float_value = s->p_value.float_value;
                break;
            case CONFIG_PARAMETER__VALUE_STRING_VALUE:
                p->string_value = s->p_value.string_value ? strdup(s->p_value.string_value) : strdup("");
                break;
            default:
                /* leave union zeroed */
                break;
            }
            /* append to plist */
            ConfigParameter **temp = realloc(s->plist, (s->n_parameters + 1) * sizeof(ConfigParameter *));
            if (!temp)
            {
                fprintf(stderr, "Failed to realloc parameter list\n");
                free(p->key);
                if (p->string_value)
                    free(p->string_value);
                free(p);
                return -1;
            }
            s->plist = temp;
            s->plist[s->n_parameters++] = p;

            /* reset current param and go back to reading sequence elements */
            reset_current_param(s);
            s->state = STATE_PARAM_MAPPING;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d in STATE_PARAM_KEY\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAM_KEYNAME:
        if (event->type == YAML_SCALAR_EVENT)
        {
            s->p_key = strdup((char *)event->data.scalar.value);
            s->state = STATE_PARAM_KEY;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d reading parameter 'key'\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAM_TYPE:
        if (event->type == YAML_SCALAR_EVENT)
        {
            /* type is numeric code in YAML (2..5) */
            int t = atoi((char *)event->data.scalar.value);
            switch (t)
            {
            case 2:
                s->p_value_case = CONFIG_PARAMETER__VALUE_BOOL_VALUE;
                break;
            case 3:
                s->p_value_case = CONFIG_PARAMETER__VALUE_INT_VALUE;
                break;
            case 4:
                s->p_value_case = CONFIG_PARAMETER__VALUE_FLOAT_VALUE;
                break;
            case 5:
                s->p_value_case = CONFIG_PARAMETER__VALUE_STRING_VALUE;
                break;
            default:
                fprintf(stderr, "Unknown parameter type: %d\n", t);
                return -1;
            }
            s->state = STATE_PARAM_KEY;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d reading parameter 'type'\n", event->type);
            return -1;
        }
        break;

    case STATE_PARAM_VALUE:
        if (event->type == YAML_SCALAR_EVENT)
        {
            char *val = (char *)event->data.scalar.value;
            switch (s->p_value_case)
            {
            case CONFIG_PARAMETER__VALUE_BOOL_VALUE:
                if (strcasecmp(val, "true") == 0 || strcmp(val, "1") == 0)
                    s->p_value.bool_value = 1;
                else
                    s->p_value.bool_value = 0;
                break;
            case CONFIG_PARAMETER__VALUE_INT_VALUE:
                s->p_value.int_value = atoi(val);
                break;
            case CONFIG_PARAMETER__VALUE_FLOAT_VALUE:
                s->p_value.float_value = (float)atof(val);
                break;
            case CONFIG_PARAMETER__VALUE_STRING_VALUE:
                /* store string; will be duplicated when pushing param */
                s->p_value.string_value = strdup(val);
                break;
            default:
                /* no type declared; treat as string fallback */
                s->p_value.string_value = strdup(val);
                s->p_value_case = CONFIG_PARAMETER__VALUE_STRING_VALUE;
                break;
            }
            s->state = STATE_PARAM_KEY;
        }
        else
        {
            fprintf(stderr, "Unexpected event %d reading parameter 'value'\n", event->type);
            return -1;
        }
        break;

    case STATE_STOP:
        /* nothing */
        break;

    default:
        fprintf(stderr, "Unhandled state %d\n", s->state);
        return -1;
    }

    return 0;
}

/* Parse a YAML file and populate a ModuleConfig structure.
   Returns 0 on success, -1 on failure. The returned ModuleConfig
   will own allocated ConfigParameter pointers and strings; caller
   must free them when done. */
int parse_module_yaml_file(const char *filename, ModuleConfig *module_config)
{
    yaml_parser_t parser;
    FILE *fh = NULL;
    if (initialize_parser(filename, &parser, &fh) < 0)
        return -1;

    struct parser_state state;
    memset(&state, 0, sizeof(state));
    state.state = STATE_START;
    state.m = (ModuleConfig)MODULE_CONFIG__INIT;
    reset_current_param(&state);

    int status = 0;
    do
    {
        yaml_event_t event;
        status = yaml_parser_parse(&parser, &event);
        if (status == 0)
        {
            fprintf(stderr, "Parser error %d\n", parser.error);
            cleanup_resources(&parser, NULL, fh);
            return -1;
        }
        status = consume_event(&state, &event);
        yaml_event_delete(&event);
        if (status < 0)
        {
            fprintf(stderr, "Failed to consume event\n");
            cleanup_resources(&parser, NULL, fh);
            return -1;
        }
    } while (state.state != STATE_STOP);

    /* cleanup parser resources */
    cleanup_resources(&parser, NULL, fh);

    /* Assign parsed values into provided ModuleConfig */
    module_config->latency_cost = state.m.latency_cost;
    module_config->energy_cost = state.m.energy_cost;
    module_config->n_parameters = state.n_parameters;
    module_config->parameters = state.plist;

    /* Note: individual ConfigParameter objects were allocated and are owned
       by module_config->parameters[]; caller must free them when appropriate. */

    return 0;
}

/* main() removed: this translation unit is now a library implementation.
   For testing, build extract_module_main.c which calls parse_module_yaml_file(). */
