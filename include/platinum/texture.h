#ifndef PLAT_TEXTURE_H
#define PLAT_TEXTURE_H

#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

PlatTexture PlatTextureCreate(PlatContext, int w, int h);
void PlatTextureDestroy(PlatTexture);
PlatTexture PlatTextureLoad(PlatContext ctx, const char* filename);

#ifdef __cplusplus
}
#endif

#endif
