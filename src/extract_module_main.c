#include <stdio.h>
#include <stdlib.h>
#include "module_config.pb-c.h"
#include "extract_module.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <module-config-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    ModuleConfig module = MODULE_CONFIG__INIT;

    if (parse_module_yaml_file(filename, &module) < 0)
    {
        fprintf(stderr, "Failed to parse module yaml %s\n", filename);
        return EXIT_FAILURE;
    }

    printf("ModuleConfig: latency_cost=%d energy_cost=%d n_parameters=%zu\n",
           module.latency_cost, module.energy_cost, module.n_parameters);
    for (size_t i = 0; i < module.n_parameters; i++)
    {
        ConfigParameter *p = module.parameters[i];
        printf(" Param %zu: key=%s type_case=%d ", i + 1, p->key ? p->key : "(null)", p->value_case);
        switch (p->value_case)
        {
        case CONFIG_PARAMETER__VALUE_BOOL_VALUE:
            printf("bool=%d\n", p->bool_value);
            break;
        case CONFIG_PARAMETER__VALUE_INT_VALUE:
            printf("int=%d\n", p->int_value);
            break;
        case CONFIG_PARAMETER__VALUE_FLOAT_VALUE:
            printf("float=%f\n", p->float_value);
            break;
        case CONFIG_PARAMETER__VALUE_STRING_VALUE:
            printf("string=%s\n", p->string_value ? p->string_value : "(null)");
            break;
        default:
            printf("no-value\n");
            break;
        }
    }

    /* Note: test program does not free allocated memory; extend if needed. */

    return EXIT_SUCCESS;
}
