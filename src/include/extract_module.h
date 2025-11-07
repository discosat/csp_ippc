#ifndef EXTRACT_MODULE_H
#define EXTRACT_MODULE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "module_config.pb-c.h"

   /* Parse a YAML file and populate a ModuleConfig structure.
      Returns 0 on success, -1 on failure. The returned ModuleConfig
      will own allocated ConfigParameter pointers and strings; caller
      must free them when done. */
   int parse_module_yaml_file(const char *filename, ModuleConfig *module_config);

#ifdef __cplusplus
}
#endif

#endif /* EXTRACT_MODULE_H */
