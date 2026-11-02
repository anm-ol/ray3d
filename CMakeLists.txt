cmake_minimum_required(VERSION 3.23)

project(ray3d LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.4-docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui)

target_include_directories(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_sources(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

find_package(OpenGL REQUIRED)
target_link_libraries(
    imgui 
    PUBLIC 
    glfw
    OpenGL::GL
)

if(UNIX AND NOT APPLE)
    find_package(X11 QUIET)

    if (X11_FOUND)  
        target_link_libraries(imgui PRIVATE X11::X11)
    endif()
endif()

add_executable(ray3d)

target_sources(ray3d
    PRIVATE
        src/main.cpp
        src/renderer.cpp
        src/world.cpp
        src/entity.cpp
        src/camera.cpp
        src/ray.cpp
        src/skybox.cpp
        src/texture.cpp
        src/bvh.cpp
        src/gui.cpp
        third_party/glad/src/glad.c
)

target_include_directories(ray3d
    PRIVATE
        third_party/glad/include
        third_party/tinyobjloader
        third_party/stb
        headers
)

target_link_libraries(ray3d
    PRIVATE
        glfw
        glm::glm
        imgui
)

target_compile_options(ray3d PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

target_compile_options(ray3d PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-g -O0 -fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(ray3d PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=address>
)
