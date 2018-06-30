cmake_minimum_required(VERSION 3.0)
project(prog)

file(GLOB_RECURSE sources src/*.cpp src/*.c src/*.h include/*.h)
file(GLOB sources-imgui ext/imgui/*.cpp ext/imgui/src/*.h)

add_executable(example ${sources} ${sources-imgui} ext/imgui/examples/imgui_impl_glfw.cpp ext/imgui/examples/imgui_impl_opengl3.cpp)

find_package(OpenGL REQUIRED)

add_subdirectory(ext/glfw)

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

target_compile_options(example PUBLIC -std=c++1y -Wall)
target_include_directories(example PUBLIC src/main)
target_include_directories(example PUBLIC include/)
target_include_directories(example PUBLIC ext/glfw/include)
target_include_directories(example PUBLIC ext/glfw/include)
target_include_directories(example PUBLIC ext/imgui)
target_link_libraries(example glfw)
target_link_libraries(example opengl32)
