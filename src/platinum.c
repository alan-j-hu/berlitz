#include <stdio.h>
#include <stdlib.h>
#include "platinum/platinum.h"
#include "platinum_impl.h"
#include "3d/3d_impl.h"

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

WGPUSwapChain PlatCreateSwapchain(PlatContext ctx, int width, int height)
{
  WGPUSwapChainDescriptor desc = {0};
  desc.nextInChain = NULL;
  desc.width = width;
  desc.height = height;
  WGPUTextureFormat format = WGPUTextureFormat_BGRA8Unorm;
  desc.format = format;
  desc.usage = WGPUTextureUsage_RenderAttachment;
  desc.presentMode = WGPUPresentMode_Fifo;
  return wgpuDeviceCreateSwapChain(ctx->device, ctx->surface, &desc);
}

PlatContext PlatContextCreate(PlatContextParams* params)
{
  PlatContext ctx = malloc(sizeof(struct PlatContextImpl));
  ctx->width = params->width;
  ctx->height = params->height;
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
    ctx->swapchain = PlatCreateSwapchain(ctx, ctx->width, ctx->height);
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

void PlatContextResize(PlatContext ctx, int w, int h)
{
  wgpuSwapChainRelease(ctx->swapchain);
  ctx->swapchain = PlatCreateSwapchain(ctx, w, h);
  ctx->width = w;
  ctx->height = h;
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
  wgpuDeviceTick(ctx->device);
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
  /* TODO: This is allocated in loop, see if I can cache or pool it */
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

void PlatEncoderDrawMesh(PlatEncoder encoder, PlatMesh mesh)
{
  wgpuRenderPassEncoderSetVertexBuffer(
    encoder->render_pass, 0,
    mesh->vertices, 0, mesh->vertices_count * sizeof(PlatVertex));
  wgpuRenderPassEncoderSetIndexBuffer(
    encoder->render_pass, mesh->indices,
    WGPUIndexFormat_Uint32, 0, mesh->indices_count * sizeof(uint32_t));
  wgpuRenderPassEncoderDrawIndexed(
    encoder->render_pass, mesh->indices_count, 1, 0, 0, 0);
}
