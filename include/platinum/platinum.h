#ifndef PLATINUM_H
#define PLATINUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "webgpu/webgpu.h"

typedef struct PlatContextImpl* PlatContext;
typedef struct PlatRenderTargetImpl* PlatRenderTarget;

PlatContext PlatCreateContext(WGPUInstance instance, WGPUSurface surface);
void PlatContextDestroy(PlatContext);
PlatRenderTarget PlatContextGetRenderTarget(PlatContext);
void PlatContextPresent(PlatContext);

void PlatRenderTargetDestroy(PlatRenderTarget);
bool PlatRenderTargetOk(PlatRenderTarget);
void PlatContextClearRenderTarget(PlatContext, PlatRenderTarget, WGPUColor);

#ifdef __cplusplus
}
#endif

#ifdef STB_PLATINUM_IMPLEMENTATION
#include <stdlib.h>

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
};

typedef struct AdapterCallbackEnv {
  WGPUAdapter adapter;
  bool ended;
} AdapterCallbackEnv;

void adapterCallback(
    WGPURequestAdapterStatus status,
    WGPUAdapter adapter,
    char const * message,
    void * userdata)
{
  AdapterCallbackEnv* env = (AdapterCallbackEnv*)(userdata);
  if (status == WGPURequestAdapterStatus_Success) {
    env->adapter = adapter;
    env->ended = true;
  } else {
  }
}

typedef struct DeviceCallbackEnv {
  WGPUDevice device;
  bool ended;
} DeviceCallbackEnv;

void deviceCallback(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    char const * message,
    void * userdata)
{
  DeviceCallbackEnv* env = (DeviceCallbackEnv*)(userdata);
  if (status == WGPURequestDeviceStatus_Success) {
    env->device = device;
    env->ended = true;
  } else {
  }
}

PlatContext PlatCreateContext(WGPUInstance instance, WGPUSurface surface)
{
  struct PlatContextImpl* ctx = malloc(sizeof(struct PlatContextImpl));
  ctx->instance = instance;
  ctx->surface = surface;

  {
    WGPURequestAdapterOptions options = {0};
    options.nextInChain = NULL;
    options.compatibleSurface = surface;
    AdapterCallbackEnv env = {0};
    wgpuInstanceRequestAdapter(instance, &options, adapterCallback, &env);
    while (!env.ended);
    ctx->adapter = env.adapter;
  }

  {
    WGPUDeviceDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.label = "Device";
    desc.requiredFeatureCount = 0;
    desc.requiredFeatures = NULL;
    desc.defaultQueue.nextInChain = NULL;
    desc.defaultQueue.label = "Queue";
    DeviceCallbackEnv env = {0};
    wgpuAdapterRequestDevice(ctx->adapter, &desc, deviceCallback, &env);
    while (!env.ended);
    ctx->device = env.device;
  }

  {

  }

  {
    WGPUSwapChainDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.width = 100;
    desc.height = 100;
    WGPUTextureFormat format = WGPUTextureFormat_BGRA8Unorm;
    desc.format = format;
    desc.usage = WGPUTextureUsage_RenderAttachment;
    desc.presentMode = WGPUPresentMode_Fifo;
    WGPUSwapChain swapchain =
      wgpuDeviceCreateSwapChain(ctx->device, ctx->surface, &desc);
    ctx->swapchain = swapchain;
  }

  return ctx;
}

void PlatContextDestroy(PlatContext ctx)
{
  wgpuSwapChainRelease(ctx->swapchain);
  wgpuDeviceRelease(ctx->device);
  wgpuAdapterRelease(ctx->adapter);
  free(ctx);
}

PlatRenderTarget PlatContextGetRenderTarget(PlatContext ctx)
{
  WGPUTextureView view = wgpuSwapChainGetCurrentTextureView(ctx->swapchain);
  PlatRenderTarget target = malloc(sizeof(struct PlatRenderTargetImpl));
  target->view = view;
  return target;
}

void PlatContextPresent(PlatContext ctx)
{
  wgpuSwapChainPresent(ctx->swapchain);
}

void PlatRenderTargetDestroy(PlatRenderTarget target)
{
  wgpuTextureViewRelease(target->view);
  free(target);
}

bool PlatRenderTargetOk(PlatRenderTarget target)
{
  return !!target->view;
}

void PlatContextClearRenderTarget(
  PlatContext ctx,
  PlatRenderTarget target,
  WGPUColor color)
{
  WGPUCommandEncoderDescriptor enc_desc = {0};
  enc_desc.nextInChain = NULL;
  enc_desc.label = "Encoder";
  WGPUCommandEncoder encoder =
    wgpuDeviceCreateCommandEncoder(ctx->device, &enc_desc);

  WGPURenderPassDescriptor desc = {0};
  desc.nextInChain = NULL;

  WGPURenderPassColorAttachment color_attachment = {0};
  color_attachment.view = target->view;
  color_attachment.resolveTarget = NULL;
  /* depth slice doesn't appear in the WebGPU header repo, but is necessary
     in Dawn's implementation */
  color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
  color_attachment.loadOp = WGPULoadOp_Clear;
  color_attachment.storeOp = WGPUStoreOp_Store;
  color_attachment.clearValue = color;
  desc.colorAttachmentCount = 1;
  desc.colorAttachments = &color_attachment;
  desc.depthStencilAttachment = NULL;

  WGPURenderPassEncoder render_pass =
    wgpuCommandEncoderBeginRenderPass(encoder, &desc);
  wgpuRenderPassEncoderEnd(render_pass);
  wgpuRenderPassEncoderRelease(render_pass);

  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);
  WGPUCommandBufferDescriptor cmd_desc = {};
  cmd_desc.nextInChain = NULL;
  cmd_desc.label = "Command buffer";
  WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmd_desc);
  wgpuQueueSubmit(queue, 1, &command);
  wgpuCommandEncoderRelease(encoder);
  wgpuCommandBufferRelease(command);
}

#endif

#endif
