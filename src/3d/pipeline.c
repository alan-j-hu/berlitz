#include <stdlib.h>
#include "cglm/vec3.h"
#include "platinum/platinum.h"
#include "../platinum_impl.h"
#include "3d_impl.h"

void PlatPipeline3DInit(PlatContext ctx, struct PlatPipeline3d* pipeline)
{
  char code[] = {
#include "3d/shader.gen"
    0x00
  };
  WGPUShaderModuleWGSLDescriptor wgsl_desc = {
    .chain = {
      .next = NULL,
      .sType = WGPUSType_ShaderModuleWGSLDescriptor
    },
    .code = code
  };

  WGPUShaderModuleDescriptor shader_desc = {
    .nextInChain = &wgsl_desc.chain,
    .label = "3d shader"
  };

  pipeline->shader_module =
    wgpuDeviceCreateShaderModule(ctx->device, &shader_desc);

  WGPUVertexAttribute vertex_attrs[1] = {0};
  vertex_attrs[0] = (struct WGPUVertexAttribute){
    .shaderLocation = 0,
    .format = WGPUVertexFormat_Float32x3,
    .offset = 0
  };

  WGPUVertexBufferLayout vert_buf_layouts[1] = {0};
  vert_buf_layouts[0] = (struct WGPUVertexBufferLayout){
    .attributeCount = 1,
    .attributes = vertex_attrs,
    .arrayStride = sizeof(struct Vertex),
    .stepMode = WGPUVertexStepMode_Vertex
  };

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

  {
    WGPUPipelineLayoutDescriptor desc = {
      .nextInChain = NULL,
      .label = "3d pipeline layout",
      /*.bindGroupLayoutCount = 1,
      .bindGroupLayouts = &pipeline->bind_group_layout*/
    };

    pipeline->pipeline_layout =
      wgpuDeviceCreatePipelineLayout(ctx->device, &desc);
  }

  WGPUBlendState blend = {
    .color= {
      .operation = WGPUBlendOperation_Add,
      .srcFactor = WGPUBlendFactor_One,
      .dstFactor = WGPUBlendFactor_One
    },
    .alpha = {
      .operation = WGPUBlendOperation_Add,
      .srcFactor = WGPUBlendFactor_One,
      .dstFactor = WGPUBlendFactor_One
    }
  };

  WGPUColorTargetState color_target = {
    .format = WGPUTextureFormat_BGRA8Unorm,
    .blend = &blend,
    .writeMask = WGPUColorWriteMask_All
  };

  WGPUFragmentState frag_state = {
    .nextInChain = NULL,
    .module = pipeline->shader_module,
    .entryPoint = "fs_main",
    .targetCount = 1,
    .targets = &color_target
  };

  WGPURenderPipelineDescriptor desc = {
    .nextInChain = NULL,
    .label = "3d pipeline",
    .layout = pipeline->pipeline_layout,
    .vertex = {
      .nextInChain = NULL,
      .module = pipeline->shader_module,
      .entryPoint = "vs_main",
      .bufferCount = 1,
      .buffers = vert_buf_layouts
    },
    .multisample = {
      .nextInChain = NULL,
      .count = 1,
      .mask = ~0u,
      .alphaToCoverageEnabled = false
    },
    .fragment = &frag_state
  };

  pipeline->render_pipeline =
    wgpuDeviceCreateRenderPipeline(ctx->device, &desc);
}

void PlatPipeline3DDeinit(struct PlatPipeline3d* pipeline)
{
  wgpuRenderPipelineRelease(pipeline->render_pipeline);
  wgpuPipelineLayoutRelease(pipeline->pipeline_layout);
  wgpuBindGroupLayoutRelease(pipeline->bind_group_layout);
  wgpuShaderModuleRelease(pipeline->shader_module);
}
