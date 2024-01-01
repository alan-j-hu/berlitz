cmake_minimum_required(VERSION 3.26)

project(Platinum LANGUAGES C CXX)

set(TINT_BUILD_TESTS OFF)
add_subdirectory(third_party)

add_executable(demo
  demo/main.cpp demo/plat_impl.c)
target_include_directories(demo PUBLIC include)
target_link_libraries(demo glfw
  dawn_glfw
  dawn_internal_config
  dawncpp
  dawn_proc
  dawn_common
  dawn_native
  dawn_wire
  dawn_utils)
