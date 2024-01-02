#include "webgpu/webgpu.h"
#include "platinum/platinum.h"
#include <GLFW/glfw3.h>
#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"
#include "webgpu/webgpu_glfw.h"

int main(int argc, char* argv[])
{
  if (!glfwInit()) {
    return 1;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(100, 100, "", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  dawnProcSetProcs(&dawn::native::GetProcs());

  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  WGPUInstance instance = wgpuCreateInstance(&desc);

  if (!instance) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  WGPUSurface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window)
    .MoveToCHandle();
  PlatContext ctx = PlatCreateContext(instance, surface);

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    PlatRenderTarget target = PlatContextGetRenderTarget(ctx);
    if (!PlatRenderTargetOk(target)) {
      PlatContextDestroy(ctx);
      wgpuInstanceRelease(instance);
      glfwDestroyWindow(window);
      glfwTerminate();
    }
    PlatContextClearRenderTarget(ctx, target, (WGPUColor){
      .r = 1.0, .g = 1.0, .b = 0.0, .a = 1.0 });
    PlatRenderTargetDestroy(target);
    PlatContextPresent(ctx);
  }

  PlatContextDestroy(ctx);

  wgpuInstanceRelease(instance);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
