#include "berlitz/3d/camera.h"
#include "cglm/cam.h"
#include "cglm/vec3.h"

struct BerlCamera3dImpl
{
  mat4 view;
  mat4 proj;
  mat4 viewproj;

  vec3 up;
  vec3 target;
  vec3 position;

  float fov_rad;
  float near_clip;
  float far_clip;

  _Bool view_dirty : 1;
  _Bool proj_dirty : 1;
};

BerlCamera3d BerlCamera3dCreate(BerlCamera3dParams* params)
{
  BerlCamera3d camera = malloc(sizeof(struct BerlCamera3dImpl));
  camera->up[0] = 0;
  camera->up[1] = 1;
  camera->up[2] = 0;

  camera->target[0] = 0;
  camera->target[1] = 0;
  camera->target[2] = 0;

  camera->position[0] = 0;
  camera->position[1] = 0;
  camera->position[2] = 0;

  camera->fov_rad = params->fov_rad;
  camera->near_clip = params->near_clip;
  camera->far_clip = params->far_clip;

  camera->view_dirty = 1;
  camera->proj_dirty = 1;

  return camera;
}

void BerlCamera3dDestroy(BerlCamera3d camera)
{
  free(camera);
}

/* Returns 0 if the vec dest was left unchanged, returns 1 otherwise. */
_Bool BerlCamera3dSetVec3(vec3 update, vec3 dest)
{
  const float x = update[0];
  const float y = update[1];
  const float z = update[2];
  if (dest[0] == x && dest[1] == y && dest[2] == z) {
    return 0;
  }
  dest[0] = x;
  dest[1] = y;
  dest[2] = z;
  return 1;
}

const vec3* BerlCamera3dPos(BerlCamera3d camera)
{
  return &camera->position;
}

void BerlCamera3dSetPos(BerlCamera3d camera, vec3 pos)
{
  camera->view_dirty |= BerlCamera3dSetVec3(pos, camera->position);
}

const vec3* BerlCamera3dTarget(BerlCamera3d camera)
{
  return &camera->target;
}

void BerlCamera3dSetTarget(BerlCamera3d camera, vec3 target)
{
  camera->view_dirty |= BerlCamera3dSetVec3(target, camera->target);
}

const mat4* BerlCamera3dView(BerlCamera3d camera)
{
  if (camera->view_dirty) {
    glm_lookat(camera->position, camera->target, camera->up, camera->view);
    camera->view_dirty = 0;
  }
  return &camera->view;
}

const mat4* BerlCamera3dProj(BerlCamera3d camera)
{
  if (camera->proj_dirty) {
    glm_perspective(
      camera->fov_rad,
      1,
      camera->near_clip,
      camera->far_clip,
      camera->proj);
    camera->proj_dirty = 0;
  }
  return &camera->proj;
}

const mat4* BerlCamera3dViewProj(BerlCamera3d camera)
{
  if (camera->view_dirty || camera->proj_dirty) {
    BerlCamera3dView(camera);
    BerlCamera3dProj(camera);
    glm_mat4_mul(camera->proj, camera->view, camera->viewproj);
  }
  return &camera->viewproj;
}
