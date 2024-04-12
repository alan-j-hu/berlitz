#ifndef BERLITZ_IMPL_H
#define BERLITZ_IMPL_H

#include "berlitz/berlitz.h"
#include "3d/3d_impl.h"

#define GLOBAL_BIND_GROUP 0
#define MATERIAL_BIND_GROUP 1
#define OBJECT_BIND_GROUP 2

struct BerlRenderTargetImpl {
  WGPUTextureView view;
};

struct BerlContextImpl {
  int width;
  int height;
  LogCallback log;

  WGPUInstance instance;
  WGPUSurface surface;
  WGPUAdapter adapter;
  WGPUDevice device;
  /* Latest WebGPU spec uses SurfaceTexture instead, but Dawn has not
     caught up */
  WGPUSwapChain swapchain;
  WGPUBuffer uniform_buffer;
  WGPUColor clear_color;
  WGPUSampler sampler;
  struct BerlPipeline3d pipeline_3d;
};

struct BerlEncoderImpl {
  WGPUCommandEncoder encoder;
  WGPURenderPassEncoder render_pass;

  WGPUBindGroup global_bind_group;
  WGPUBuffer camera_buffer;
};

struct BerlTextureImpl {
  WGPUTexture texture;
  WGPUTextureView view;
};

struct BerlMaterialImpl {
  WGPUSampler sampler;
  WGPUBindGroup bind_group;
};

struct BerlObjectDataImpl {
  mat4 transform_matrix;
  WGPUBindGroup bind_group;
  WGPUBuffer transform_buffer;
};

#endif
