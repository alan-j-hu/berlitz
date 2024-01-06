cmake_minimum_required(VERSION 3.26)

project(Platinum LANGUAGES C CXX)

set(TINT_BUILD_TESTS OFF)
add_subdirectory(third_party)

add_custom_command(
  OUTPUT
  "${CMAKE_CURRENT_BINARY_DIR}/src/shader.gen.c"
  COMMAND
  "sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/embed.sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shader.wgsl"
  "${CMAKE_CURRENT_BINARY_DIR}/src/shader.gen.c"
  DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/embed.sh"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shader.wgsl")

add_custom_target(generate_shaders ALL
  DEPENDS
  "${CMAKE_CURRENT_BINARY_DIR}/src/shader.gen.c")

add_library(platinum
  src/platinum.c
  "${CMAKE_CURRENT_BINARY_DIR}/src/shader.gen.c")
add_dependencies(platinum generate_shaders)
target_include_directories(platinum PUBLIC include)
target_link_libraries(platinum
  dawn_common)

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
