#include "berlitz/3d/mesh.h"
#include "../berlitz_impl.h"
#include "3d_impl.h"
#include <memory.h>

BerlMesh BerlMeshCreate(
  BerlContext ctx,
  BerlVertex3d* vertices, size_t vc,
  uint32_t* indices, size_t ic)
{
  BerlMesh mesh = malloc(sizeof(struct BerlMeshImpl));
  WGPUQueue queue = wgpuDeviceGetQueue(ctx->device);

  WGPUBufferDescriptor vdesc = {
    .nextInChain = NULL,
    .label = "",
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
    .size = vc * sizeof(BerlVertex3d),
    .mappedAtCreation = false
  };
  WGPUBuffer vbuf = wgpuDeviceCreateBuffer(ctx->device, &vdesc);
  wgpuQueueWriteBuffer(queue, vbuf, 0, vertices, vdesc.size);
  mesh->vertices = vbuf;
  mesh->vertices_count = vc;

  WGPUBufferDescriptor idesc = {
    .nextInChain = NULL,
    .label = "",
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
    .size = ic * sizeof(uint32_t),
    .mappedAtCreation = false
  };
  WGPUBuffer ibuf = wgpuDeviceCreateBuffer(ctx->device, &idesc);
  wgpuQueueWriteBuffer(queue, ibuf, 0, indices, idesc.size);
  mesh->indices = ibuf;
  mesh->indices_count = ic;

  return mesh;
}

void BerlMeshDestroy(BerlMesh mesh)
{
  wgpuBufferDestroy(mesh->vertices);
  wgpuBufferRelease(mesh->vertices);
  wgpuBufferDestroy(mesh->indices);
  wgpuBufferRelease(mesh->indices);
  free(mesh);
}
