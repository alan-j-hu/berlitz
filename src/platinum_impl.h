#ifndef PLATINUM_IMPL_H
#define PLATINUM_IMPL_H

#include "platinum/platinum.h"
#include "3d/3d_impl.h"

struct PlatRenderTargetImpl {
  WGPUTextureView view;
};

struct PlatContextImpl {
  LogCallback log;

  WGPUInstance instance;
  WGPUSurface surface;
  WGPUAdapter adapter;
  WGPUDevice device;
  /* Latest WebGPU spec uses SurfaceTexture instead, but Dawn has not
     caught up */
  WGPUSwapChain swapchain;
  WGPUBuffer uniform_buffer;

  struct PlatPipeline3d pipeline_3d;
};

#endif
