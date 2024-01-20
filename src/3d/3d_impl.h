#ifndef PLATINUM_3D_IMPL_H
#define PLATINUM_3D_IMPL_H

#include "cglm/vec3.h"

struct Vertex {
  vec3 pos;
};

struct PlatPipeline3d {
  WGPUShaderModule shader_module;
  WGPUBindGroupLayout bind_group_layout;
  WGPUPipelineLayout pipeline_layout;
  WGPURenderPipeline render_pipeline;
};

void PlatPipeline3DInit(PlatContext ctx, struct PlatPipeline3d* pipeline);
void PlatPipeline3DDeinit(struct PlatPipeline3d* pipeline);

#endif
