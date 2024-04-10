#ifndef BERLITZ_3D_MESH_H
#define BERLITZ_3D_MESH_H

#include "../core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BerlVertex3d {
  vec3 pos;
  vec2 tex_coord;
} BerlVertex3d;

BerlMesh BerlMeshCreate(
  BerlContext ctx,
  BerlVertex3d* vertices, size_t vertices_count,
  uint32_t* indices, size_t indices_count);
void BerlMeshDestroy(BerlMesh);

#ifdef __cplusplus
}
#endif

#endif
