/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: pipeline_config.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "include/csp_pipeline_config/pipeline_config.pb-c.h"
void   module_definition__init
                     (ModuleDefinition         *message)
{
  static const ModuleDefinition init_value = MODULE_DEFINITION__INIT;
  *message = init_value;
}
size_t module_definition__get_packed_size
                     (const ModuleDefinition *message)
{
  assert(message->base.descriptor == &module_definition__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t module_definition__pack
                     (const ModuleDefinition *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &module_definition__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t module_definition__pack_to_buffer
                     (const ModuleDefinition *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &module_definition__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
ModuleDefinition *
       module_definition__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (ModuleDefinition *)
     protobuf_c_message_unpack (&module_definition__descriptor,
                                allocator, len, data);
}
void   module_definition__free_unpacked
                     (ModuleDefinition *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &module_definition__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   pipeline_definition__init
                     (PipelineDefinition         *message)
{
  static const PipelineDefinition init_value = PIPELINE_DEFINITION__INIT;
  *message = init_value;
}
size_t pipeline_definition__get_packed_size
                     (const PipelineDefinition *message)
{
  assert(message->base.descriptor == &pipeline_definition__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t pipeline_definition__pack
                     (const PipelineDefinition *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &pipeline_definition__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t pipeline_definition__pack_to_buffer
                     (const PipelineDefinition *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &pipeline_definition__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
PipelineDefinition *
       pipeline_definition__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (PipelineDefinition *)
     protobuf_c_message_unpack (&pipeline_definition__descriptor,
                                allocator, len, data);
}
void   pipeline_definition__free_unpacked
                     (PipelineDefinition *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &pipeline_definition__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor module_definition__field_descriptors[3] =
{
  {
    "order",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(ModuleDefinition, order),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "name",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(ModuleDefinition, name),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "param_id",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(ModuleDefinition, param_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned module_definition__field_indices_by_name[] = {
  1,   /* field[1] = name */
  0,   /* field[0] = order */
  2,   /* field[2] = param_id */
};
static const ProtobufCIntRange module_definition__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor module_definition__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "ModuleDefinition",
  "ModuleDefinition",
  "ModuleDefinition",
  "",
  sizeof(ModuleDefinition),
  3,
  module_definition__field_descriptors,
  module_definition__field_indices_by_name,
  1,  module_definition__number_ranges,
  (ProtobufCMessageInit) module_definition__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor pipeline_definition__field_descriptors[1] =
{
  {
    "modules",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(PipelineDefinition, n_modules),
    offsetof(PipelineDefinition, modules),
    &module_definition__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned pipeline_definition__field_indices_by_name[] = {
  0,   /* field[0] = modules */
};
static const ProtobufCIntRange pipeline_definition__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor pipeline_definition__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "PipelineDefinition",
  "PipelineDefinition",
  "PipelineDefinition",
  "",
  sizeof(PipelineDefinition),
  1,
  pipeline_definition__field_descriptors,
  pipeline_definition__field_indices_by_name,
  1,  pipeline_definition__number_ranges,
  (ProtobufCMessageInit) pipeline_definition__init,
  NULL,NULL,NULL    /* reserved[123] */
};
