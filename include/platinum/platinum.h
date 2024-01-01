#ifndef PLATINUM_H
#define PLATINUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "webgpu/webgpu.h"

typedef struct PlatContextImpl* PlatContext;

PlatContext PlatCreateContext(WGPUInstance instance);

void PlatDestroyContext(PlatContext);

#ifdef __cplusplus
}
#endif

#ifdef STB_PLATINUM_IMPLEMENTATION
#include <stdbool.h>
#include <stdlib.h>

struct PlatContextImpl {
  WGPUInstance instance;
  WGPUAdapter adapter;
  WGPUDevice device;
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

PlatContext PlatCreateContext(WGPUInstance instance)
{
  struct PlatContextImpl* ctx = malloc(sizeof(struct PlatContextImpl));
  ctx->instance = instance;

  {
    WGPURequestAdapterOptions options = {0};
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
  };

  return ctx;
}

void PlatDestroyContext(PlatContext ctx)
{
  wgpuDeviceRelease(ctx->device);
  wgpuAdapterRelease(ctx->adapter);
  free(ctx);
}
#endif

#endif
