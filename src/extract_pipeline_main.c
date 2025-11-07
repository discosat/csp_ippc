#include <stdio.h>
#include <stdlib.h>
#include "pipeline_config.pb-c.h"
#include "extract_pipeline.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <pipeline-config-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    PipelineDefinition pipeline = PIPELINE_DEFINITION__INIT;

    if (parse_pipeline_yaml_file(filename, &pipeline) < 0)
    {
        fprintf(stderr, "Failed to parse pipeline yaml %s\n", filename);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < pipeline.n_modules; i++)
    {
        ModuleDefinition *mod = pipeline.modules[i];
        printf("Module %zu: %s with %zu implementations\n",
               i + 1,
               mod->name,
               mod->n_implementations);
        for (size_t j = 0; j < mod->n_implementations; j++)
        {
            Implementation *impl = mod->implementations[j];
            printf("  Implementation %zu: param_id=%d effort_level=%d\n",
                   j + 1,
                   impl->param_id,
                   impl->effort_level);
        }
    }

    return EXIT_SUCCESS;
}
