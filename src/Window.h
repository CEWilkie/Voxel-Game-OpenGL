//
// Created by cew05 on 18/04/2024.
//

#ifndef UNTITLED7_WINDOW_H
#define UNTITLED7_WINDOW_H

#include <SDL.h>
#include <vector>
#include <glew.h>

#include "CoreGlobals.h"
#include "Shaders.h"

class Window {
    private:
        // Window info vars
        SDL_Rect winRect {};
        float aspectRatio {};

        // Window object and contex
        SDL_Window* window {};
        SDL_GLContext glContext {};

        // Shader programs
        unsigned int shader = 0;

    public:
        // Setup
        Window();
        ~Window();
        bool CreateGLContext();
        unsigned int CreateShaders();

        // Setters
        void SetWindowSize(int _w, int _h);

        // Getters
        SDL_Window* WindowPtr();
        void GetWindowSize(int& _w, int& _h) const;
        [[nodiscard]] unsigned int GetShader() const { return shader; };
        [[nodiscard]] float GetAspectRatio() const { return aspectRatio; };
};

inline Window window;

#endif //UNTITLED7_WINDOW_H
