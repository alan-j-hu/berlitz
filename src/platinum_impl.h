#ifndef PLATINUM_IMPL_H
#define PLATINUM_IMPL_H

#include "platinum/platinum.h"
#include "3d/3d_impl.h"

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

  struct PlatPipeline3d pipeline_3d;
};

struct PlatEncoderImpl {
  WGPUCommandEncoder encoder;
  WGPURenderPassEncoder render_pass;
};

#endif
