# Pretty much just https://github.com/Pesc0/imgui-cmake/blob/master/libs/CMakeLists.txt modified to use GLFW instead of SDL

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

target_sources( IMGUI
                PRIVATE
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

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED PATHS "C:/Program Files\ (x86)/GLFW/lib/cmake/glfw3")
include_directories("C:/Program Files\ (x86)/GLFW/include")


#if(BUILD_EXAMPLES)
    set(SOURCE_FILES ${IMGUI_DIR}/main.cpp)
    add_executable(imgui_glfw_example)
    target_sources(imgui_glfw_example PUBLIC ${IMGUI_DIR}/main.cpp)
    target_link_libraries(imgui_glfw_example IMGUI Opengl32 glfw)
    set_target_properties(imgui_glfw_example PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#endif()
