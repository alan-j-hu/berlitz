#include "berlitz/texture.h"
#include "berlitz_impl.h"
#include "stb_image.h"

BerlTexture BerlTextureCreate(BerlContext ctx, int w, int h)
{
  BerlTexture berl_texture = malloc(sizeof(struct BerlTextureImpl));
  WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm;

  WGPUTextureDescriptor desc = {
    .nextInChain = NULL,
    .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
    .dimension = WGPUTextureDimension_2D,
    .size = {
      .width = w,
      .height = h,
      .depthOrArrayLayers = 1
    },
    .format = format,
    .mipLevelCount = 1,
    .sampleCount = 1,
    .viewFormatCount = 0,
    .viewFormats = NULL
  };

  WGPUTexture texture = wgpuDeviceCreateTexture(ctx->device, &desc);

  WGPUTextureViewDescriptor view_desc = {
    .nextInChain = NULL,
    .label = NULL,
    .format = format,
    .dimension = WGPUTextureViewDimension_2D,
    .baseMipLevel = 0,
    .mipLevelCount = 1,
    .baseArrayLayer = 0,
    .arrayLayerCount = 1,
    .aspect = WGPUTextureAspect_All
  };

  WGPUTextureView view = wgpuTextureCreateView(texture, &view_desc);

  berl_texture->texture = texture;
  berl_texture->view = view;

  return berl_texture;
}

void BerlTextureUpload(
  BerlContext ctx, BerlTexture berl_texture, void* data)
{
  int w = wgpuTextureGetWidth(berl_texture->texture);
  int h = wgpuTextureGetHeight(berl_texture->texture);
  WGPUImageCopyTexture dst = {
    .nextInChain = NULL,
    .texture = berl_texture->texture,
    .mipLevel = 0,
    .origin = {
      .x = 0,
      .y = 0,
      .z = 0
    },
    .aspect = WGPUTextureAspect_All
  };

  WGPUTextureDataLayout src = {
    .nextInChain = NULL,
    .offset = 0,
    .bytesPerRow = 4 * w,
    .rowsPerImage = h
  };

  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);
  WGPUExtent3D extent = {
    .width = w,
    .height = h,
    .depthOrArrayLayers = 1
  };
  wgpuQueueWriteTexture(queue, &dst, data, 4 * w * h, &src, &extent);
}

BerlTexture BerlTextureLoad(BerlContext ctx, const char* filename)
{
  int w, h, channels;
  stbi_uc* pixels = stbi_load(filename, &w, &h, &channels, 4);
  BerlTexture berl_texture = BerlTextureCreate(ctx, w, h);
  BerlTextureUpload(ctx, berl_texture, pixels);
  stbi_image_free(pixels);
  return berl_texture;
}

void BerlTextureDestroy(BerlTexture berl_texture)
{
  wgpuTextureViewRelease(berl_texture->view);
  wgpuTextureDestroy(berl_texture->texture);
  wgpuTextureRelease(berl_texture->texture);
  free(berl_texture);
}
