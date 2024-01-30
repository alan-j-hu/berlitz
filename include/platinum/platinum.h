#ifndef PLATINUM_H
#define PLATINUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "cglm/vec3.h"
#include "webgpu/webgpu.h"

typedef struct PlatContextImpl* PlatContext;
typedef struct PlatRenderTargetImpl* PlatRenderTarget;
typedef struct PlatEncoderImpl* PlatEncoder;
typedef struct PlatMeshImpl* PlatMesh;
typedef struct PlatMeshBuilderImpl* PlatMeshBuilder;
typedef struct PlatTextureImpl* PlatTexture;

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

PlatEncoder PlatEncoderCreate(PlatContext, PlatRenderTarget);
void PlatEncoderDestroy(PlatContext, PlatEncoder);
void PlatEncoderDrawMesh(PlatEncoder, PlatMesh);

typedef struct PlatVertex {
  vec3 pos;
} PlatVertex;

PlatMesh PlatMeshCreate(
  PlatContext ctx,
  PlatVertex* vertices, size_t vertices_count,
  uint32_t* indices, size_t indices_count);
void PlatMeshDestroy(PlatMesh);

PlatTexture PlatTextureCreate(PlatContext, int w, int h);
void PlatTextureDestroy(PlatTexture);
PlatTexture PlatTextureLoad(PlatContext ctx, const char* filename);

#ifdef __cplusplus
}
#endif

#endif
