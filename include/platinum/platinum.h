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

typedef void (*LogCallback)(const char* message);

typedef struct PlatContextParams {
  WGPUInstance instance;
  WGPUSurface surface;
  WGPUColor clear_color;
  LogCallback log;
} PlatContextParams;

PlatContext PlatCreateContext(PlatContextParams*);
void PlatContextDestroy(PlatContext);
void PlatContextPresent(PlatContext);

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

#ifdef __cplusplus
}
#endif

#endif
