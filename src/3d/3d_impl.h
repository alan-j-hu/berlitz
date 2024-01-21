#ifndef PLATINUM_3D_IMPL_H
#define PLATINUM_3D_IMPL_H

struct PlatPipeline3d {
  WGPUShaderModule shader_module;
  WGPUBindGroupLayout bind_group_layout;
  WGPUPipelineLayout pipeline_layout;
  WGPURenderPipeline render_pipeline;
};

void PlatPipeline3DInit(PlatContext ctx, struct PlatPipeline3d* pipeline);
void PlatPipeline3DDeinit(struct PlatPipeline3d* pipeline);

struct PlatMeshImpl {
  WGPUBuffer vertices;
  WGPUBuffer indices;
};

struct PlatMeshBuilderImpl {
};

#endif
