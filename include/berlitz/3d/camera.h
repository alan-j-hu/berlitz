#ifndef BERLITZ_3D_CAMERA_H
#define BERLITZ_3D_CAMERA_H

#include "../core.h"
#include "cglm/mat4.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BerlCamera3dParams {
  float fov_rad;
  float near_clip;
  float far_clip;
} BerlCamera3dParams;

BerlCamera3d BerlCamera3dCreate(BerlCamera3dParams*);
void BerlCamera3dDestroy(BerlCamera3d);

const vec3* BerlCamera3dPos(BerlCamera3d);
void BerlCamera3dSetPos(BerlCamera3d, vec3 pos);
const vec3* BerlCamera3dTarget(BerlCamera3d);
void BerlCamera3dSetTarget(BerlCamera3d, vec3 target);

const mat4* BerlCamera3dView(BerlCamera3d);
const mat4* BerlCamera3dProj(BerlCamera3d);
const mat4* BerlCamera3dViewProj(BerlCamera3d);

#ifdef __cplusplus
}
#endif

#endif
