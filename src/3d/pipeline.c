#include <stddef.h>
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

  WGPUVertexAttribute vertex_attrs[2];
  vertex_attrs[0] = (struct WGPUVertexAttribute){
    .shaderLocation = 0,
    .format = WGPUVertexFormat_Float32x3,
    .offset = offsetof(PlatVertex, pos)
  };
  vertex_attrs[1] = (struct WGPUVertexAttribute){
    .shaderLocation = 1,
    .format = WGPUVertexFormat_Float32x2,
    .offset = offsetof(PlatVertex, tex_coord)
  };

  WGPUVertexBufferLayout vert_buf_layouts[1] = {0};
  vert_buf_layouts[0] = (struct WGPUVertexBufferLayout){
    .attributeCount = 2,
    .attributes = vertex_attrs,
    .arrayStride = sizeof(PlatVertex),
    .stepMode = WGPUVertexStepMode_Vertex
  };

  {
    WGPUBindGroupLayoutEntry entries[1] = {0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Fragment;
    entries[0].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 1;
    layout_desc.entries = entries;
    pipeline->sampler_bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  {
    WGPUBindGroupLayoutEntry entries[1] = {0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Fragment;
    entries[0].texture.sampleType = WGPUTextureSampleType_Float;
    entries[0].texture.viewDimension = WGPUTextureViewDimension_2D;

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 1;
    layout_desc.entries = entries;
    pipeline->texture_bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  WGPUBindGroupLayout layouts[] = {
    pipeline->sampler_bind_group_layout,
    pipeline->texture_bind_group_layout
  };

  {
    WGPUPipelineLayoutDescriptor desc = {
      .nextInChain = NULL,
      .label = "3d pipeline layout",
      .bindGroupLayoutCount = 2,
      .bindGroupLayouts = layouts
    };

    pipeline->pipeline_layout =
      wgpuDeviceCreatePipelineLayout(ctx->device, &desc);
  }

  /* TODO: Learn about different blend formulas */
  WGPUBlendState blend = {
    .color= {
      .operation = WGPUBlendOperation_Add,
      .srcFactor = WGPUBlendFactor_Src,
      .dstFactor = WGPUBlendFactor_Zero
    },
    .alpha = {
      .operation = WGPUBlendOperation_Add,
      .srcFactor = WGPUBlendFactor_Src,
      .dstFactor = WGPUBlendFactor_Zero
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
    .primitive = {
      .topology = WGPUPrimitiveTopology_TriangleList,
      .stripIndexFormat = WGPUIndexFormat_Undefined,
      .frontFace = WGPUFrontFace_CCW,
      .cullMode = WGPUCullMode_None,
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

  {
    WGPUBindGroupEntry bindings[1] = {0};
    bindings[0].nextInChain = NULL;
    bindings[0].binding = 0;
    bindings[0].sampler = ctx->sampler;

    WGPUBindGroupDescriptor bind_group_desc = {
      .nextInChain = NULL,
      .label = NULL,
      .layout = pipeline->sampler_bind_group_layout,
      .entryCount = 1,
      .entries = bindings,
    };
    WGPUBindGroup sampler_bind_group =
      wgpuDeviceCreateBindGroup(ctx->device, &bind_group_desc);
    pipeline->sampler_bind_group = sampler_bind_group;
  }
}

void PlatPipeline3DDeinit(struct PlatPipeline3d* pipeline)
{
  wgpuBindGroupRelease(pipeline->sampler_bind_group);
  wgpuRenderPipelineRelease(pipeline->render_pipeline);
  wgpuPipelineLayoutRelease(pipeline->pipeline_layout);
  wgpuBindGroupLayoutRelease(pipeline->texture_bind_group_layout);
  wgpuBindGroupLayoutRelease(pipeline->sampler_bind_group_layout);
  wgpuShaderModuleRelease(pipeline->shader_module);
}
