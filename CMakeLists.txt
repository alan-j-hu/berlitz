cmake_minimum_required(VERSION 3.26)

project(Platinum LANGUAGES C CXX)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

set(TINT_BUILD_TESTS OFF)
add_subdirectory(third_party)

add_custom_command(
  OUTPUT
  "${CMAKE_CURRENT_BINARY_DIR}/src/3d/shader.gen"
  COMMAND
  "sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/embed.sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/3d/shader.wgsl"
  "${CMAKE_CURRENT_BINARY_DIR}/src/3d/shader.gen"
  DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/embed.sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/3d/shader.wgsl")

add_custom_target(generate_shaders ALL
  DEPENDS
  "${CMAKE_CURRENT_BINARY_DIR}/src/3d/shader.gen")

add_library(platinum
  src/platinum.c
  src/stb_image.c
  src/texture.c
  src/3d/camera.c
  src/3d/mesh.c
  src/3d/pipeline.c)
add_dependencies(platinum generate_shaders)
target_include_directories(platinum PUBLIC include
  PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/src"
  third_party/stb)
target_link_libraries(platinum
  cglm
  dawn_common
  SDL2::SDL2)

add_custom_target(demo_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/demo/res/ $<TARGET_FILE_DIR:demo>/res/)
add_executable(demo
  demo/main.cpp)
target_include_directories(demo PUBLIC include)
target_link_libraries(demo platinum glfw
  dawn_glfw
  dawn_internal_config
  dawncpp
  dawn_proc
  dawn_common
  dawn_native
  dawn_utils)
add_dependencies(demo demo_resources)
