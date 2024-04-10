#include <stdio.h>
#include <stdlib.h>
#include "berlitz/berlitz.h"
#include "berlitz_impl.h"
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
  BerlContext ctx = (BerlContext) userdata;
  ctx->log(message);
}

WGPUSwapChain BerlCreateSwapchain(BerlContext ctx, int width, int height)
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

BerlContext BerlContextCreate(BerlContextParams* params)
{
  BerlContext ctx = malloc(sizeof(struct BerlContextImpl));
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
    ctx->swapchain = BerlCreateSwapchain(ctx, ctx->width, ctx->height);
  }

  {
    WGPUSamplerDescriptor desc = {
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Linear,
      .minFilter = WGPUFilterMode_Linear,
      .mipmapFilter = WGPUMipmapFilterMode_Linear,
      .lodMinClamp = 0.0f,
      .lodMaxClamp = 1.0f,
      .compare = WGPUCompareFunction_Undefined,
      .maxAnisotropy = 1
    };
    WGPUSampler sampler = wgpuDeviceCreateSampler(ctx->device, &desc);
    ctx->sampler = sampler;
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

  BerlPipeline3DInit(ctx, &ctx->pipeline_3d);

  return ctx;
}

void BerlContextDestroy(BerlContext ctx)
{
  BerlPipeline3DDeinit(&ctx->pipeline_3d);
  wgpuSamplerRelease(ctx->sampler);
  wgpuSwapChainRelease(ctx->swapchain);
  wgpuDeviceRelease(ctx->device);
  wgpuAdapterRelease(ctx->adapter);
  free(ctx);
}

void BerlContextResize(BerlContext ctx, int w, int h)
{
  wgpuSwapChainRelease(ctx->swapchain);
  ctx->swapchain = BerlCreateSwapchain(ctx, w, h);
  ctx->width = w;
  ctx->height = h;
}

BerlRenderTarget BerlContextGetRenderTarget(BerlContext ctx)
{
  WGPUTextureView view = wgpuSwapChainGetCurrentTextureView(ctx->swapchain);
  BerlRenderTarget target = malloc(sizeof(struct BerlRenderTargetImpl));
  target->view = view;
  return target;
}

void BerlContextPresent(BerlContext ctx)
{
  wgpuSwapChainPresent(ctx->swapchain);
  wgpuDeviceTick(ctx->device);
}

void BerlRenderTargetDestroy(BerlRenderTarget target)
{
  wgpuTextureViewRelease(target->view);
  free(target);
}

bool BerlRenderTargetOk(BerlRenderTarget target)
{
  return !!target->view;
}

BerlMaterial BerlMaterialCreate(BerlContext ctx, BerlTexture plat_texture)
{
  BerlMaterial material = malloc(sizeof(struct BerlMaterialImpl));

  WGPUBindGroupEntry bindings[1] = {0};
  bindings[0].nextInChain = NULL;
  bindings[0].binding = 0;
  bindings[0].textureView = plat_texture->view;

  WGPUBindGroupDescriptor bind_group_desc = {
    .nextInChain = NULL,
    .label = NULL,
    .layout = ctx->pipeline_3d.material_bind_group_layout,
    .entryCount = 1,
    .entries = bindings,
  };
  WGPUBindGroup bind_group =
    wgpuDeviceCreateBindGroup(ctx->device, &bind_group_desc);

  material->bind_group = bind_group;

  return material;
}

void BerlMaterialDestroy(BerlMaterial material)
{
  wgpuBindGroupRelease(material->bind_group);
  free(material);
}

BerlObjectData BerlObjectDataCreate(BerlContext ctx)
{
  BerlObjectData data = malloc(sizeof(struct BerlObjectDataImpl));

  WGPUBufferDescriptor buffer_desc = {0};
  buffer_desc.size = sizeof(mat4);
  buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
  buffer_desc.mappedAtCreation = 0;
  WGPUBuffer buffer = wgpuDeviceCreateBuffer(ctx->device, &buffer_desc);

  WGPUBindGroupEntry entries[1] = {0, 0};
  entries[0].nextInChain = NULL;
  entries[0].binding = 0;
  entries[0].buffer = buffer;
  entries[0].offset = 0;
  entries[0].size = sizeof(mat4);

  WGPUBindGroupDescriptor bind_group_desc = {
    .nextInChain = NULL,
    .label = NULL,
    .layout = ctx->pipeline_3d.object_bind_group_layout,
    .entryCount = 1,
    .entries = entries,
  };
  WGPUBindGroup bind_group =
    wgpuDeviceCreateBindGroup(ctx->device, &bind_group_desc);

  data->bind_group = bind_group;
  data->transform_buffer = buffer;

  return data;
}

void BerlObjectDataDestroy(BerlObjectData data)
{
  wgpuBufferDestroy(data->transform_buffer);
  wgpuBufferRelease(data->transform_buffer);
  wgpuBindGroupRelease(data->bind_group);
  free(data);
}

BerlEncoder BerlEncoderCreate(BerlContext ctx)
{
  BerlEncoder berl_encoder = malloc(sizeof(struct BerlEncoderImpl));

  WGPUBufferDescriptor buffer_desc = {0};
  buffer_desc.size = sizeof(mat4);
  buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
  buffer_desc.mappedAtCreation = 0;
  berl_encoder->camera_buffer =
    wgpuDeviceCreateBuffer(ctx->device, &buffer_desc);

  WGPUBindGroupEntry entries[2] = {0, 0};
  entries[0].nextInChain = NULL;
  entries[0].binding = 0;
  entries[0].sampler = ctx->sampler;

  entries[1].nextInChain = NULL;
  entries[1].binding = 1;
  entries[1].buffer = berl_encoder->camera_buffer;
  entries[1].offset = 0;
  entries[1].size = sizeof(mat4);

  WGPUBindGroupDescriptor bind_group_desc = {
    .nextInChain = NULL,
    .label = NULL,
    .layout = ctx->pipeline_3d.global_bind_group_layout,
    .entryCount = 2,
    .entries = entries,
  };
  berl_encoder->global_bind_group =
    wgpuDeviceCreateBindGroup(ctx->device, &bind_group_desc);

  return berl_encoder;
}

void BerlEncoderBegin(
  BerlContext ctx,
  BerlEncoder berl_encoder,
  const mat4 viewproj,
  BerlRenderTarget target)
{
  WGPUCommandEncoderDescriptor enc_desc = {0};
  enc_desc.nextInChain = NULL;
  enc_desc.label = "Encoder";
  WGPUCommandEncoder encoder =
    wgpuDeviceCreateCommandEncoder(ctx->device, &enc_desc);
  berl_encoder->encoder = encoder;

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
  berl_encoder->render_pass = render_pass;

  wgpuRenderPassEncoderSetBindGroup(
    render_pass,
    GLOBAL_BIND_GROUP,
    berl_encoder->global_bind_group,
    0,
    NULL);

  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);
  wgpuQueueWriteBuffer(
    queue, berl_encoder->camera_buffer, 0, viewproj, sizeof(mat4));
}

void BerlEncoderEnd(BerlContext ctx, BerlEncoder encoder)
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
}

void BerlEncoderDestroy(BerlEncoder encoder)
{
  wgpuBufferDestroy(encoder->camera_buffer);
  wgpuBufferRelease(encoder->camera_buffer);
  wgpuBindGroupRelease(encoder->global_bind_group);
  free(encoder);
}

void BerlEncoderSetMaterial(BerlEncoder encoder, BerlMaterial material)
{
  wgpuRenderPassEncoderSetBindGroup(
    encoder->render_pass, MATERIAL_BIND_GROUP, material->bind_group, 0, NULL);
}

void BerlEncoderDrawMesh(
  BerlContext ctx,
  BerlEncoder encoder,
  BerlObjectData data,
  BerlMesh mesh)
{
  wgpuRenderPassEncoderSetBindGroup(
    encoder->render_pass,
    OBJECT_BIND_GROUP,
    data->bind_group,
    0,
    NULL);

  glm_mat4_identity(data->transform_matrix);
  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);
  wgpuQueueWriteBuffer(
    queue, data->transform_buffer, 0, data->transform_matrix, sizeof(mat4));

  wgpuRenderPassEncoderSetVertexBuffer(
    encoder->render_pass, 0,
    mesh->vertices, 0, mesh->vertices_count * sizeof(BerlVertex3d));
  wgpuRenderPassEncoderSetIndexBuffer(
    encoder->render_pass, mesh->indices,
    WGPUIndexFormat_Uint32, 0, mesh->indices_count * sizeof(uint32_t));
  wgpuRenderPassEncoderDrawIndexed(
    encoder->render_pass, mesh->indices_count, 1, 0, 0, 0);
}
