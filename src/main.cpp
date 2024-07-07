#define SDL_MAIN_HANDLED
#include <iostream>
#include <random>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>

#include "Window.h"
#include "Triangle.h"

int main(int argc, char** argv){
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LogError("Failed to init", SDL_GetError(), true);
        return 0;
    }

    // Construct Window Object
    Window window;
    if (window.WindowPtr() == nullptr) return 0;        // Create SDL_Window
    if (!window.CreateGLContext()) return 0;            // Create openGL context in SDL_Window

    // Init glew : requires openGL context to have been made
    if (glewInit() != GLEW_OK) {
        LogError("Failed to initialise glew", gluErrorString(glGetError()), true);
        return 0;
    }

    // Create shaders
    unsigned int shaderID;
    if ((shaderID = window.CreateShaders()) == 0) return 0;
    glUseProgram(shaderID);

    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<std::unique_ptr<Triangle>> triangles;

    for (int t = 0; t < 2; t++) {
        // Create Triangle
        triangles.push_back(std::make_unique<Triangle>());
        triangles[t]->ConstructTriangle();
    }
    printf("TRIANGLES: %zu\n", triangles.size());


    std::unique_ptr<Triangle> stillTriangle = std::make_unique<Triangle>();
    stillTriangle->ConstructTriangle();

    // Render Loop
    bool running = true;

    Uint64 frameStart;
    while (running) {
        frameStart = SDL_GetTicks64();

        // CLEAR

        glClearColor(0.1f, 1.0f, 0.6f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT);

        // DRAW

        for (const auto& t : triangles) {
            t->Display();
        }
        stillTriangle->Display();

        // MOVE

        for (const auto& to : triangles) {
            to->Move();
        }

        // INPUTS

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // update display
        SDL_GL_SwapWindow(window.WindowPtr());

        Uint64 frametime = SDL_GetTicks64() - frameStart;
        if (frametime < 1000/60) {
            SDL_Delay(1000/60 - frametime);
        }
    }

    // end handling
    SDL_Quit();

    return 0;
}