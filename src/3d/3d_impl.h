#ifndef BERLITZ_3D_IMPL_H
#define BERLITZ_3D_IMPL_H

#include "cglm/vec3.h"

struct BerlPipeline3d {
  WGPUShaderModule shader_module;
  WGPUBindGroupLayout global_bind_group_layout;
  WGPUBindGroupLayout material_bind_group_layout;
  WGPUBindGroupLayout object_bind_group_layout;
  WGPUPipelineLayout pipeline_layout;
  WGPURenderPipeline render_pipeline;
};

void BerlPipeline3DInit(BerlContext ctx, struct BerlPipeline3d* pipeline);
void BerlPipeline3DDeinit(struct BerlPipeline3d* pipeline);

struct BerlMeshImpl {
  size_t vertices_count;
  WGPUBuffer vertices;
  size_t indices_count;
  WGPUBuffer indices;
};

struct BerlCameraImpl {
  vec3 pos;
  vec3 target;
};

#endif
