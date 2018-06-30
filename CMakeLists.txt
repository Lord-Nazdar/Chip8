cmake_minimum_required(VERSION 3.0)
project(prog)

file(GLOB_RECURSE sources src/*.cpp src/*.c src/*.h include/*.h)
file(GLOB sources-imgui ext/imgui/*.cpp ext/imgui/src/*.h)
file(GLOB_RECURSE source_gl3w ext/gl3w/include/*.h ext/gl3w/src/gl3w.c)

source_group(imgui FILES ${sources-imgui} ext/imgui/examples/imgui_impl_glfw.cpp
ext/imgui/examples/imgui_impl_opengl3.cpp)

source_group(gl3w FILES ${source_gl3w})

add_executable(example ${sources} ${sources-imgui} ext/imgui/examples/imgui_impl_glfw.cpp ext/imgui/examples/imgui_impl_opengl3.cpp ${source_gl3w})

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(ext/glfw)
target_link_libraries(example glfw)
target_include_directories(example PUBLIC ext/glfw/include)

target_compile_options(example PUBLIC -std=c++1y -Wall)
target_include_directories(example PUBLIC src/main)
target_include_directories(example PUBLIC include/)
target_include_directories(example PUBLIC ${GLFW_Include})
target_include_directories(example PUBLIC ext/glfw/include)
target_include_directories(example PUBLIC ext/imgui)
target_include_directories(example PUBLIC ext/gl3w/include)
target_link_libraries(example opengl32)
