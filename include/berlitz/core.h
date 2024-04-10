#ifndef BERLITZ_CORE_H
#define BERLITZ_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "cglm/mat4.h"
#include "cglm/vec3.h"
#include "webgpu/webgpu.h"

typedef struct BerlContextImpl* BerlContext;
typedef struct BerlRenderTargetImpl* BerlRenderTarget;
typedef struct BerlEncoderImpl* BerlEncoder;
typedef struct BerlMeshImpl* BerlMesh;
typedef struct BerlMaterialImpl* BerlMaterial;
typedef struct BerlTextureImpl* BerlTexture;
typedef struct BerlObjectDataImpl* BerlObjectData;
typedef struct BerlCamera3dImpl* BerlCamera3d;

typedef void (*LogCallback)(const char* message);

typedef struct BerlContextParams {
  int width;
  int height;
  WGPUInstance instance;
  WGPUSurface surface;
  WGPUColor clear_color;
  LogCallback log;
} BerlContextParams;

BerlContext BerlContextCreate(BerlContextParams*);
void BerlContextDestroy(BerlContext);
void BerlContextPresent(BerlContext);
void BerlContextResize(BerlContext, int w, int h);

BerlRenderTarget BerlContextGetRenderTarget(BerlContext);
void BerlRenderTargetDestroy(BerlRenderTarget);
bool BerlRenderTargetOk(BerlRenderTarget);

BerlMaterial BerlMaterialCreate(BerlContext, BerlTexture);
void BerlMaterialDestroy(BerlMaterial);

BerlObjectData BerlObjectDataCreate(BerlContext ctx);
void BerlObjectDataDestroy(BerlObjectData);

BerlEncoder BerlEncoderCreate(BerlContext);
void BerlEncoderDestroy(BerlEncoder);
void BerlEncoderBegin(BerlContext, BerlEncoder, const mat4, BerlRenderTarget);
void BerlEncoderEnd(BerlContext, BerlEncoder);
void BerlEncoderSetMaterial(BerlEncoder, BerlMaterial);
void BerlEncoderDrawMesh(BerlContext, BerlEncoder, BerlObjectData, BerlMesh);

#ifdef __cplusplus
}
#endif

#endif
