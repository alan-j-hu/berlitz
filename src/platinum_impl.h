#ifndef PLATINUM_IMPL_H
#define PLATINUM_IMPL_H

#include "platinum/platinum.h"
#include "3d/3d_impl.h"

#define GLOBAL_BIND_GROUP 0
#define MATERIAL_BIND_GROUP 1
#define OBJECT_BIND_GROUP 2

struct PlatRenderTargetImpl {
  WGPUTextureView view;
};

struct PlatContextImpl {
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
  struct PlatPipeline3d pipeline_3d;
};

struct PlatEncoderImpl {
  WGPUCommandEncoder encoder;
  WGPURenderPassEncoder render_pass;

  WGPUBindGroup global_bind_group;
  WGPUBuffer camera_buffer;
};

struct PlatTextureImpl {
  WGPUTexture texture;
  WGPUTextureView view;
};

struct PlatMaterialImpl {
  WGPUBindGroup bind_group;
};

struct PlatObjectDataImpl {
  mat4 transform_matrix;
  WGPUBindGroup bind_group;
  WGPUBuffer transform_buffer;
};

#endif
