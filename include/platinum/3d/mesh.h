#ifndef PLAT_3D_MESH_H
#define PLAT_3D_MESH_H

#include "../core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlatVertex3d {
  vec3 pos;
  vec2 tex_coord;
} PlatVertex3d;

PlatMesh PlatMeshCreate(
  PlatContext ctx,
  PlatVertex3d* vertices, size_t vertices_count,
  uint32_t* indices, size_t indices_count);
void PlatMeshDestroy(PlatMesh);

#ifdef __cplusplus
}
#endif

#endif
