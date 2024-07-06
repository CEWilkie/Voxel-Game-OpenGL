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
        SDL_Rect winRect {};
        SDL_Window* window {};
        SDL_Renderer* renderer {};
        SDL_GLContext glContext {};

        unsigned int shader = 0;

    public:
        // Setup
        Window();
        bool CreateGLContext();
        unsigned int CreateShaders();

        // Closure
        void EndWindow();

        // Setters
        void SetWindowSize(int _w, int _h);

        // Getters
        SDL_Window* WindowPtr();
};

#endif //UNTITLED7_WINDOW_H
