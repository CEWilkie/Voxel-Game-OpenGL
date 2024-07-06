//
// Created by cew05 on 18/04/2024.
//

#include "Window.h"

Window::Window() {
    winRect = {0, 0, 500, 500};
    int b_top, b_left, b_right, b_bottom;

    // Create SDL window object
    window = SDL_CreateWindow("openGlWindow", 0, 0, 500, 500, SDL_WINDOW_OPENGL);
    if (!window) {
        LogError("Failed to create window", SDL_GetError(), true);
        return;
    }

    // Attempt to fetch window border size
    if (SDL_GetWindowBordersSize(window, &b_top, &b_left, &b_bottom, &b_right) != 0) {
        LogError("Failed to retrive display borders size", SDL_GetError(), false);
    }

    // Set window position
    winRect.x += b_left;
    winRect.y += b_top;
    SDL_SetWindowPosition(window, winRect.x, winRect.y);
}

Window::~Window() {
    glDeleteProgram(shader);
    SDL_DestroyWindow(window);
}

bool Window::CreateGLContext() {
    // set openGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // openGl context
    SDL_GLContext openGlContext = SDL_GL_CreateContext(window);
    if (!openGlContext) {
        LogError("Failed to create openGL context for window", SDL_GetError(), true);
        return false;
    }

    return true;
}

unsigned int Window::CreateShaders() {
    // First load shaders from filenames
    std::string vertexShader = LoadShaderSourceFromFile("../src_shader/vertex.glsl");
    std::string fragmentShader = LoadShaderSourceFromFile("../src_shader/fragment.glsl");

    shader = CreateShader(vertexShader, fragmentShader);
    return shader;
}

void Window::SetWindowSize(int _w, int _h) {
    _w = std::max(500, _w);
    _h = std::max(500, _h);

    winRect.w = _w;
    winRect.h = _h;
    SDL_SetWindowSize(window, winRect.w, winRect.h);
}

SDL_Window* Window::WindowPtr() {
    return window;
}

void Window::GetWindowSize(int& _w, int& _h) const {
    _w = winRect.w;
    _h = winRect.h;
}