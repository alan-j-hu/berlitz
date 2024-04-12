#include <stddef.h>
#include <stdlib.h>
#include "cglm/mat4.h"
#include "berlitz/berlitz.h"
#include "../berlitz_impl.h"
#include "3d_impl.h"

void BerlPipeline3DInit(BerlContext ctx, struct BerlPipeline3d* pipeline)
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
    .offset = offsetof(BerlVertex3d, pos)
  };
  vertex_attrs[1] = (struct WGPUVertexAttribute){
    .shaderLocation = 1,
    .format = WGPUVertexFormat_Float32x2,
    .offset = offsetof(BerlVertex3d, tex_coord)
  };

  WGPUVertexBufferLayout vert_buf_layouts[1] = {0};
  vert_buf_layouts[0] = (struct WGPUVertexBufferLayout){
    .attributeCount = 2,
    .attributes = vertex_attrs,
    .arrayStride = sizeof(BerlVertex3d),
    .stepMode = WGPUVertexStepMode_Vertex
  };

  {
    WGPUBindGroupLayoutEntry entries[1] = {0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Vertex;
    entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    entries[0].buffer.minBindingSize = sizeof(mat4);

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 1;
    layout_desc.entries = entries;
    pipeline->global_bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  {
    WGPUBindGroupLayoutEntry entries[2] = {0, 0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Fragment;
    entries[0].texture.sampleType = WGPUTextureSampleType_Float;
    entries[0].texture.viewDimension = WGPUTextureViewDimension_2D;

    entries[1].binding = 1;
    entries[1].visibility = WGPUShaderStage_Fragment;
    entries[1].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 2;
    layout_desc.entries = entries;
    pipeline->material_bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  {
    WGPUBindGroupLayoutEntry entries[1] = {0};
    entries[0].binding = 0;
    entries[0].visibility = WGPUShaderStage_Vertex;
    entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    entries[0].buffer.minBindingSize = sizeof(mat4);

    WGPUBindGroupLayoutDescriptor layout_desc = {0};
    layout_desc.nextInChain = NULL;
    layout_desc.entryCount = 1;
    layout_desc.entries = entries;
    pipeline->object_bind_group_layout =
      wgpuDeviceCreateBindGroupLayout(ctx->device, &layout_desc);
  }

  WGPUBindGroupLayout layouts[3];
  layouts[GLOBAL_BIND_GROUP] = pipeline->global_bind_group_layout;
  layouts[MATERIAL_BIND_GROUP] = pipeline->material_bind_group_layout;
  layouts[OBJECT_BIND_GROUP] = pipeline->object_bind_group_layout;

  {
    WGPUPipelineLayoutDescriptor desc = {
      .nextInChain = NULL,
      .label = "3d pipeline layout",
      .bindGroupLayoutCount = 3,
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
}

void BerlPipeline3DDeinit(struct BerlPipeline3d* pipeline)
{
  wgpuRenderPipelineRelease(pipeline->render_pipeline);
  wgpuPipelineLayoutRelease(pipeline->pipeline_layout);
  wgpuBindGroupLayoutRelease(pipeline->object_bind_group_layout);
  wgpuBindGroupLayoutRelease(pipeline->material_bind_group_layout);
  wgpuBindGroupLayoutRelease(pipeline->global_bind_group_layout);
  wgpuShaderModuleRelease(pipeline->shader_module);
}
