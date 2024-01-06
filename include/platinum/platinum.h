#ifndef PLATINUM_H
#define PLATINUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "webgpu/webgpu.h"

typedef struct PlatContextImpl* PlatContext;
typedef struct PlatRenderTargetImpl* PlatRenderTarget;

PlatContext PlatCreateContext(WGPUInstance instance, WGPUSurface surface);
void PlatContextDestroy(PlatContext);
PlatRenderTarget PlatContextGetRenderTarget(PlatContext);
void PlatContextPresent(PlatContext);

void PlatRenderTargetDestroy(PlatRenderTarget);
bool PlatRenderTargetOk(PlatRenderTarget);
void PlatContextClearRenderTarget(PlatContext, PlatRenderTarget, WGPUColor);

#ifdef __cplusplus
}
#endif

#endif
