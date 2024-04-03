#ifndef PLATINUM_3D_IMPL_H
#define PLATINUM_3D_IMPL_H

#include "cglm/vec3.h"

struct PlatPipeline3d {
  WGPUShaderModule shader_module;
  WGPUBindGroupLayout sampler_bind_group_layout;
  WGPUBindGroupLayout camera_bind_group_layout;
  WGPUBindGroupLayout texture_bind_group_layout;
  WGPUPipelineLayout pipeline_layout;
  WGPURenderPipeline render_pipeline;

  WGPUBindGroup sampler_bind_group;
};

void PlatPipeline3DInit(PlatContext ctx, struct PlatPipeline3d* pipeline);
void PlatPipeline3DDeinit(struct PlatPipeline3d* pipeline);

struct PlatMeshImpl {
  size_t vertices_count;
  WGPUBuffer vertices;
  size_t indices_count;
  WGPUBuffer indices;
};

struct PlatCameraImpl {
  vec3 pos;
  vec3 target;
};

#endif
