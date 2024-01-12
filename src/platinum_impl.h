#ifndef PLATINUM_IMPL_H
#define PLATINUM_IMPL_H

#include "platinum/platinum.h"

struct PlatRenderTargetImpl {
  WGPUTextureView view;
};

struct PlatContextImpl {
  WGPUInstance instance;
  WGPUSurface surface;
  WGPUAdapter adapter;
  WGPUDevice device;
  /* Latest WebGPU spec uses SurfaceTexture instead, but Dawn has not
     caught up */
  WGPUSwapChain swapchain;
  WGPUBuffer uniform_buffer;
};

#endif
