#ifndef BERLITZ_TEXTURE_H
#define BERLITZ_TEXTURE_H

#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

BerlTexture BerlTextureCreate(BerlContext, int w, int h);
void BerlTextureDestroy(BerlTexture);
BerlTexture BerlTextureLoad(BerlContext ctx, const char* filename);

#ifdef __cplusplus
}
#endif

#endif
