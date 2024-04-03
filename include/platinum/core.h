#ifndef PLAT_CORE_H
#define PLAT_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "webgpu/webgpu.h"

typedef struct PlatContextImpl* PlatContext;
typedef struct PlatRenderTargetImpl* PlatRenderTarget;
typedef struct PlatEncoderImpl* PlatEncoder;
typedef struct PlatMeshImpl* PlatMesh;
typedef struct PlatMeshBuilderImpl* PlatMeshBuilder;
typedef struct PlatTextureImpl* PlatTexture;
typedef struct PlatCamera3dImpl* PlatCamera3d;

typedef void (*LogCallback)(const char* message);

typedef struct PlatContextParams {
  int width;
  int height;
  WGPUInstance instance;
  WGPUSurface surface;
  WGPUColor clear_color;
  LogCallback log;
} PlatContextParams;

PlatContext PlatContextCreate(PlatContextParams*);
void PlatContextDestroy(PlatContext);
void PlatContextPresent(PlatContext);
void PlatContextResize(PlatContext, int w, int h);

PlatRenderTarget PlatContextGetRenderTarget(PlatContext);
void PlatRenderTargetDestroy(PlatRenderTarget);
bool PlatRenderTargetOk(PlatRenderTarget);

PlatEncoder PlatEncoderCreate(PlatContext);
void PlatEncoderDestroy(PlatEncoder);
void PlatEncoderBegin(PlatContext, PlatEncoder, mat4, PlatRenderTarget);
void PlatEncoderEnd(PlatContext, PlatEncoder);
void PlatEncoderDrawMesh(PlatContext, PlatEncoder, PlatMesh, PlatTexture);

#ifdef __cplusplus
}
#endif

#endif
