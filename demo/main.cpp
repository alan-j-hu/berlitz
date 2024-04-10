#include "webgpu/webgpu.h"
#include "berlitz/berlitz.h"
#include <cmath>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"
#include "webgpu/webgpu_glfw.h"

void debug_log(const char* message)
{
  std::cerr << message << std::flush;
}

void on_window_resize(GLFWwindow* window, int w, int h)
{
  BerlContext ctx = static_cast<BerlContext>(glfwGetWindowUserPointer(window));
  BerlContextResize(ctx, w, h);
}

int main(int argc, char* argv[])
{
  const double PI = 4 * std::atan(1);
  BerlCamera3dParams camera_params {};
  camera_params.fov_rad = PI / 4;
  camera_params.near_clip = 0.1;
  camera_params.far_clip = 5.0;
  BerlCamera3d camera = BerlCamera3dCreate(&camera_params);

  vec3 camera_pos;
  camera_pos[0] = 0;
  camera_pos[1] = 0;
  camera_pos[2] = -1;
  BerlCamera3dSetPos(camera, camera_pos);

  vec3 camera_target;
  camera_target[0] = 0;
  camera_target[1] = 0;
  camera_target[2] = 0;
  BerlCamera3dSetTarget(camera, camera_target);

  if (!glfwInit()) {
    BerlCamera3dDestroy(camera);
    return 1;
  }
  if (SDL_Init(0) < 0) {
    glfwTerminate();
    BerlCamera3dDestroy(camera);
    return 1;
  }

  char* base_path = SDL_GetBasePath();
  std::cerr << base_path << std::endl;
  std::filesystem::path base = base_path;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(100, 100, "", NULL, NULL);
  if (!window) {
    SDL_free(base_path);
    SDL_Quit();
    glfwTerminate();
    BerlCamera3dDestroy(camera);
    return 1;
  }

  dawnProcSetProcs(&dawn::native::GetProcs());

  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  WGPUInstance instance = wgpuCreateInstance(&desc);

  if (!instance) {
    SDL_free(base_path);
    SDL_Quit();
    glfwDestroyWindow(window);
    glfwTerminate();
    BerlCamera3dDestroy(camera);
    return 1;
  }

  WGPUSurface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window)
    .MoveToCHandle();
  BerlContextParams params {};
  params.width = 100;
  params.height = 100;
  params.instance = instance;
  params.surface = surface;
  params.clear_color = (WGPUColor){
    .r = 1.0, .g = 1.0, .b = 0.0, .a = 1.0 };
  params.log = debug_log;
  BerlContext ctx = BerlContextCreate(&params);

  glfwSetWindowUserPointer(window, ctx);
  glfwSetFramebufferSizeCallback(window, on_window_resize);

  BerlTexture tex = BerlTextureLoad(ctx, (base / "res/cat.png").c_str());
  BerlMaterial material = BerlMaterialCreate(ctx, tex);

  BerlVertex3d vertices[4];
  vec3 v1 = {-0.5f, -0.5f, 0.5f};
  std::memcpy(vertices[0].pos, v1, sizeof(vec3));
  vec2 t1 = {0.0, 1.0};
  std::memcpy(vertices[0].tex_coord, t1, sizeof(vec2));

  vec3 v2 = {+0.5f, -0.5f, 0.5f};
  std::memcpy(vertices[1].pos, v2, sizeof(vec3));
  vec2 t2 = {1.0, 1.0};
  std::memcpy(vertices[1].tex_coord, t2, sizeof(vec2));

  vec3 v3 = {+0.5f, +0.5f, 1.0f};
  std::memcpy(vertices[2].pos, v3, sizeof(vec3));
  vec2 t3 = {1.0, 0.0};
  std::memcpy(vertices[2].tex_coord, t3, sizeof(vec2));

  vec3 v4 = {-0.5f, +0.5f, 1.0f};
  std::memcpy(vertices[3].pos, v4, sizeof(vec3));
  vec2 t4 = {0.0, 0.0};
  std::memcpy(vertices[3].tex_coord, t4, sizeof(vec2));

  uint32_t indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  BerlMesh mesh = BerlMeshCreate(ctx, vertices, 4, indices, 6);
  BerlObjectData obj = BerlObjectDataCreate(ctx);
  BerlEncoder encoder = BerlEncoderCreate(ctx);

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    BerlRenderTarget target = BerlContextGetRenderTarget(ctx);
    if (!BerlRenderTargetOk(target)) {
      BerlEncoderDestroy(encoder);
      BerlObjectDataDestroy(obj);
      BerlMeshDestroy(mesh);
      BerlMaterialDestroy(material);
      BerlTextureDestroy(tex);
      BerlContextDestroy(ctx);
      wgpuInstanceRelease(instance);
      glfwDestroyWindow(window);
      glfwTerminate();
      BerlCamera3dDestroy(camera);
      return 1;
    }

    BerlEncoderBegin(ctx, encoder, *BerlCamera3dViewProj(camera), target);
    BerlEncoderSetMaterial(encoder, material);
    BerlEncoderDrawMesh(ctx, encoder, obj, mesh);
    BerlEncoderEnd(ctx, encoder);
    BerlRenderTargetDestroy(target);
    BerlContextPresent(ctx);
  }

  BerlEncoderDestroy(encoder);
  BerlObjectDataDestroy(obj);
  BerlMeshDestroy(mesh);

  BerlMaterialDestroy(material);
  BerlTextureDestroy(tex);
  BerlContextDestroy(ctx);

  wgpuInstanceRelease(instance);

  SDL_free(base_path);
  SDL_Quit();
  glfwDestroyWindow(window);
  glfwTerminate();
  BerlCamera3dDestroy(camera);
  return 0;
}
