#include <stdio.h>
#include <stdlib.h>
#include "platinum/platinum.h"
#include "platinum_impl.h"

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

void deviceUncapturedErrorCallback(
  WGPUErrorType type, char const * message, void * userdata)
{
  PlatContext ctx = (PlatContext) userdata;
  ctx->log(message);
}

PlatContext PlatCreateContext(PlatContextParams* params)
{
  PlatContext ctx = malloc(sizeof(struct PlatContextImpl));
  ctx->instance = params->instance;
  ctx->surface = params->surface;
  ctx->clear_color = params->clear_color;

  {
    WGPURequestAdapterOptions options = {0};
    options.nextInChain = NULL;
    options.compatibleSurface = ctx->surface;
    AdapterCallbackEnv env = {0};
    wgpuInstanceRequestAdapter(ctx->instance, &options, adapterCallback, &env);
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

  ctx->log = params->log;
  if (ctx->log != NULL) {
    wgpuDeviceSetUncapturedErrorCallback(
      ctx->device, deviceUncapturedErrorCallback, ctx);
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

  {
    WGPUBufferDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.size = sizeof(float);
    desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    desc.mappedAtCreation = false;
    WGPUBuffer buffer = wgpuDeviceCreateBuffer(ctx->device, &desc);
    ctx->uniform_buffer = buffer;
  }

  PlatPipeline3DInit(ctx, &ctx->pipeline_3d);

  return ctx;
}

void PlatContextDestroy(PlatContext ctx)
{
  PlatPipeline3DDeinit(&ctx->pipeline_3d);

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

PlatEncoder PlatEncoderCreate(PlatContext ctx, PlatRenderTarget target)
{
  PlatEncoder plat_encoder = malloc(sizeof(struct PlatEncoderImpl));

  WGPUCommandEncoderDescriptor enc_desc = {0};
  enc_desc.nextInChain = NULL;
  enc_desc.label = "Encoder";
  WGPUCommandEncoder encoder =
    wgpuDeviceCreateCommandEncoder(ctx->device, &enc_desc);
  plat_encoder->encoder = encoder;

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
  color_attachment.clearValue = ctx->clear_color;
  desc.colorAttachmentCount = 1;
  desc.colorAttachments = &color_attachment;
  desc.depthStencilAttachment = NULL;

  WGPURenderPassEncoder render_pass =
    wgpuCommandEncoderBeginRenderPass(encoder, &desc);
  wgpuRenderPassEncoderSetPipeline(
    render_pass, ctx->pipeline_3d.render_pipeline);
  plat_encoder->render_pass = render_pass;
  return plat_encoder;
}

void PlatEncoderDestroy(PlatContext ctx, PlatEncoder encoder)
{
  wgpuRenderPassEncoderEnd(encoder->render_pass);
  wgpuRenderPassEncoderRelease(encoder->render_pass);

  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);
  WGPUCommandBufferDescriptor cmd_desc = {0};
  cmd_desc.nextInChain = NULL;
  cmd_desc.label = "Command buffer";
  WGPUCommandBuffer command =
    wgpuCommandEncoderFinish(encoder->encoder, &cmd_desc);
  wgpuQueueSubmit(queue, 1, &command);
  wgpuCommandEncoderRelease(encoder->encoder);
  wgpuCommandBufferRelease(command);
  free(encoder);
}
