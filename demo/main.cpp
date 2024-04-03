#include "webgpu/webgpu.h"
#include "platinum/platinum.h"
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
  PlatContext ctx = static_cast<PlatContext>(glfwGetWindowUserPointer(window));
  PlatContextResize(ctx, w, h);
}

int main(int argc, char* argv[])
{
  mat4 viewproj;
  glm_mat4_identity(viewproj);
  viewproj[0][0] = 2.0;
  viewproj[1][1] = 0.5;

  if (!glfwInit()) {
    return 1;
  }
  if (SDL_Init(0) < 0) {
    glfwTerminate();
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
    return 1;
  }

  WGPUSurface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window)
    .MoveToCHandle();
  PlatContextParams params {};
  params.width = 100;
  params.height = 100;
  params.instance = instance;
  params.surface = surface;
  params.clear_color = (WGPUColor){
    .r = 1.0, .g = 1.0, .b = 0.0, .a = 1.0 };
  params.log = debug_log;
  PlatContext ctx = PlatContextCreate(&params);

  glfwSetWindowUserPointer(window, ctx);
  glfwSetFramebufferSizeCallback(window, on_window_resize);

  PlatTexture tex = PlatTextureLoad(ctx, (base / "res/cat.png").c_str());

  PlatVertex3d vertices[4];
  vec3 v1 = {-0.5f, -0.5f, 0.5f};
  std::memcpy(vertices[0].pos, v1, sizeof(vec3));
  vec2 t1 = {0.0, 1.0};
  std::memcpy(vertices[0].tex_coord, t1, sizeof(vec2));

  vec3 v2 = {+0.5f, -0.5f, 0.5f};
  std::memcpy(vertices[1].pos, v2, sizeof(vec3));
  vec2 t2 = {1.0, 1.0};
  std::memcpy(vertices[1].tex_coord, t2, sizeof(vec2));

  vec3 v3 = {+0.5f, +0.5f, 0.5f};
  std::memcpy(vertices[2].pos, v3, sizeof(vec3));
  vec2 t3 = {1.0, 0.0};
  std::memcpy(vertices[2].tex_coord, t3, sizeof(vec2));

  vec3 v4 = {-0.5f, +0.5f, 0.5f};
  std::memcpy(vertices[3].pos, v4, sizeof(vec3));
  vec2 t4 = {0.0, 0.0};
  std::memcpy(vertices[3].tex_coord, t4, sizeof(vec2));

  uint32_t indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  PlatMesh mesh = PlatMeshCreate(ctx, vertices, 4, indices, 6);
  PlatEncoder encoder = PlatEncoderCreate(ctx);

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    PlatRenderTarget target = PlatContextGetRenderTarget(ctx);
    if (!PlatRenderTargetOk(target)) {
      PlatEncoderDestroy(encoder);
      PlatMeshDestroy(mesh);
      PlatContextDestroy(ctx);
      wgpuInstanceRelease(instance);
      glfwDestroyWindow(window);
      glfwTerminate();
    }

    PlatEncoderBegin(ctx, encoder, viewproj, target);
    PlatEncoderDrawMesh(ctx, encoder, mesh, tex);
    PlatEncoderEnd(ctx, encoder);
    PlatRenderTargetDestroy(target);
    PlatContextPresent(ctx);
  }

  PlatEncoderDestroy(encoder);
  PlatMeshDestroy(mesh);

  PlatTextureDestroy(tex);
  PlatContextDestroy(ctx);

  wgpuInstanceRelease(instance);

  SDL_free(base_path);
  SDL_Quit();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
