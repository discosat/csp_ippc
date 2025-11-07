#ifndef EXTRACT_PIPELINE_H
#define EXTRACT_PIPELINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "pipeline_config.pb-c.h"

    /* Parse a YAML file and populate a PipelineDefinition structure.
        Returns 0 on success, -1 on failure. */
    int parse_pipeline_yaml_file(const char *filename, PipelineDefinition *pipeline);

#ifdef __cplusplus
}
#endif

#endif /* EXTRACT_PIPELINE_H */
