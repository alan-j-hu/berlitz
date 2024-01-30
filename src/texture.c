#include "platinum/platinum.h"
#include "platinum_impl.h"
#include "stb_image.h"

PlatTexture PlatTextureCreate(PlatContext ctx, int w, int h)
{
  PlatTexture plat_texture = malloc(sizeof(struct PlatTextureImpl));

  WGPUTextureDescriptor desc = {
    .nextInChain = NULL,
    .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
    .dimension = WGPUTextureDimension_2D,
    .size = {
      .width = w,
      .height = h,
      .depthOrArrayLayers = 1
    },
    .format = WGPUTextureFormat_RGBA8Unorm,
    .mipLevelCount = 1,
    .sampleCount = 1,
    .viewFormatCount = 0,
    .viewFormats = NULL
  };

  WGPUTexture texture = wgpuDeviceCreateTexture(ctx->device, &desc);

  plat_texture->texture = texture;

  return plat_texture;
}

void PlatTextureUpload(
  PlatContext ctx, PlatTexture plat_texture, void* data)
{
  int w = wgpuTextureGetWidth(plat_texture->texture);
  int h = wgpuTextureGetHeight(plat_texture->texture);
  WGPUImageCopyTexture dst = {
    .nextInChain = NULL,
    .texture = plat_texture->texture,
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

PlatTexture PlatTextureLoad(PlatContext ctx, const char* filename)
{
  int w, h, channels;
  stbi_uc* pixels = stbi_load(filename, &w, &h, &channels, 4);
  PlatTexture plat_texture = PlatTextureCreate(ctx, w, h);
  PlatTextureUpload(ctx, plat_texture, pixels);
  stbi_image_free(pixels);
  return plat_texture;
}

void PlatTextureDestroy(PlatTexture plat_texture)
{
  wgpuTextureDestroy(plat_texture->texture);
  wgpuTextureRelease(plat_texture->texture);
  free(plat_texture);
}