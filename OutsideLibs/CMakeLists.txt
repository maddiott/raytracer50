# GLFW this has to be first since imgui depends on it
set(glfw_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.8)

# Starter CMAKE for building GLFW from within a project
# https://github.com/juliettef/GLFW-CMake-starter/blob/main/CMakeLists.txt
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_EXAMPLES OFF)
set( GLFW_BUILD_TESTS OFF)

find_package(OpenGL REQUIRED)

add_subdirectory(glfw-3.3.8)
target_include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.8/include)
include_directories(glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.8/include)

# Pretty much just https://github.com/Pesc0/imgui-cmake/blob/master/libs/CMakeLists.txt modified to use GLFW instead of SDL
# IMGUI
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

target_sources( IMGUI
                PUBLIC
                    ${IMGUI_DIR}/imgui_demo.cpp
                    ${IMGUI_DIR}/imgui_draw.cpp
                    ${IMGUI_DIR}/imgui_tables.cpp
                    ${IMGUI_DIR}/imgui_widgets.cpp
                    ${IMGUI_DIR}/imgui.cpp
                    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
                    ${IMGUI_DIR}/imgui_impl_glfw.cpp
                )

target_include_directories( IMGUI
                            PUBLIC ${IMGUI_DIR}
                            )

set(BUILD_EXAMPLES false)

if(BUILD_EXAMPLES)
    set(SOURCE_FILES ${IMGUI_DIR}/main.cpp)
    add_executable(imgui_glfw_example)
    target_sources(imgui_glfw_example PUBLIC ${IMGUI_DIR}/main.cpp)
    target_link_libraries(imgui_glfw_example IMGUI Opengl32 glfw)
    set_target_properties(imgui_glfw_example PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# image writer file
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb_image_writer)
include_directories( STB
                     PUBLIC ${STB_DIR}
                     )

