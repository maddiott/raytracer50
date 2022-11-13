cmake_minimum_required(VERSION 3.20)
project(raytrace50 VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

#find_package(glfw3 3.3 REQUIRED PATHS "C:/Program Files\ (x86)/GLFW/lib/cmake/glfw3")

add_subdirectory("OutsideLibs")
add_subdirectory("lib")

#find_package(glfw 3.3)
target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/OutsideLibs/glfw-3.3.8/include)
#include_directories("C:/Program Files\ (x86)/GLFW/include")
find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp)

add_executable(raytrace50 ${SOURCE_FILES})
target_link_libraries(raytrace50 glfw ${OPENGL_LIBRARIES} IMGUI RAYTRACER)
