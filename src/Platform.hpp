#pragma once

#include <glad.h>
#include <SDL.h>
#include <map>

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture{};
    
    // Modern OpenGL objects
    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;

    std::map<SDL_Keycode, uint8_t> keyMap;
};