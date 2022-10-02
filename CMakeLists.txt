cmake_minimum_required(VERSION 3.20)
project(raytrace50 VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 3.3 REQUIRED PATHS "C:/Program Files\ (x86)/GLFW/lib/cmake/glfw3")
include_directories("C:/Program Files\ (x86)/GLFW/include")


add_subdirectory("OutsideLibs")

set(SOURCE_FILES main.cpp)

add_executable(raytrace50 ${SOURCE_FILES})
target_link_libraries(raytrace50 glfw Opengl32 IMGUI)
