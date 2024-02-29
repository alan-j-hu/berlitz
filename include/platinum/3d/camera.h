#ifndef PLAT_3D_CAMERA_H
#define PLAT_3D_CAMERA_H

#include "../core.h"
#include "cglm/mat4.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlatCamera3dParams {
  float fov_rad;
  float near_clip;
  float far_clip;
} PlatCamera3dParams;

PlatCamera3d PlatCamera3dCreate(PlatCamera3dParams*);
void PlatCamera3dDestroy(PlatCamera3d);

const vec3* PlatCamera3dPos(PlatCamera3d);
void PlatCamera3dSetPos(PlatCamera3d, vec3 pos);
const vec3* PlatCamera3dTarget(PlatCamera3d);
void PlatCamera3dSetTarget(PlatCamera3d, vec3 target);

const mat4* PlatCamera3dView(PlatCamera3d);
const mat4* PlatCamera3dProj(PlatCamera3d);
const mat4* PlatCamera3dViewProj(PlatCamera3d);

#ifdef __cplusplus
}
#endif

#endif
