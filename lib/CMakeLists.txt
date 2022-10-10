set(RAYTRACER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_library(RAYTRACER STATIC)
include_directories(${RAYTRACER_DIR}/../include)

include_directories(${RAYTRACER_DIR}/../OutsideLibs/imgui
                    ${RAYTRACER_DIR}/../OutsideLibs/stb_image_writer
                    )

target_sources(RAYTRACER
               PRIVATE
               RaytracerApp.cpp
               Camera.cpp
               Viewport.cpp
               World.cpp
               )
