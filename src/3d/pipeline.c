#include <stdlib.h>
#include "platinum/platinum.h"
#include "../platinum_impl.h"
#include "shader.h"

struct Pipeline3D {
  WGPUBindGroupLayout bind_group_layout;
  WGPUPipelineLayout pipeline_layout;
};

void PlatPipeline3D(PlatContext ctx, struct Pipeline3D* pipeline)
{
  {
    char* c = shader;
  }

  {
    WGPUBindGroupLayoutEntry entries[1] = {0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Vertex;
    entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    entries[0].buffer.minBindingSize = 0;

    WGPUBindGroupEntry bindings[1] = {0};
    bindings[0].nextInChain = NULL;
    bindings[0].binding = 0;
    bindings[0].buffer = ctx->uniform_buffer;
    bindings[0].offset = 0;
    bindings[0].size = 0;

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 1;
    layout_desc.entries = entries;
    pipeline->bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  WGPUPipelineLayoutDescriptor desc = {0};
  desc.nextInChain = NULL;
  desc.bindGroupLayoutCount = 1;
  desc.bindGroupLayouts = &pipeline->bind_group_layout;
  pipeline->pipeline_layout =
    wgpuDeviceCreatePipelineLayout(ctx->device, &desc);
}
