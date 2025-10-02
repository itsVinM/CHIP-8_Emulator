cmake_minimum_required(VERSION 3.14)

project(chip8 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(
    chip8
    src/Chip8.cpp
    src/Main.cpp
    src/Platform.cpp
    3rdParty/glad/src/glad.c
)

target_sources(chip8 PRIVATE
    3rdParty/imgui-1.88/imgui.cpp
    3rdParty/imgui-1.88/imgui_draw.cpp
    3rdParty/imgui-1.88/imgui_widgets.cpp
    3rdParty/imgui-1.88/imgui_tables.cpp
    3rdParty/imgui-1.88/backends/imgui_impl_sdl2.cpp
    3rdParty/imgui-1.88/backends/imgui_impl_opengl3.cpp
)

target_include_directories(chip8 PRIVATE 
    3rdParty/glad/include
    3rdParty/imgui-1.88
    3rdParty/imgui-1.88/backends
)

target_compile_options(chip8 PRIVATE -Wall -Wextra)
target_link_libraries(chip8 PRIVATE SDL2::SDL2)