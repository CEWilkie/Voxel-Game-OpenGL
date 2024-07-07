#define SDL_MAIN_HANDLED
#include <iostream>
#include <random>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>

#include <glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "Triangle.h"
#include "Quad.h"
#include "Cube.h"

int main(int argc, char** argv){
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LogError("Failed to init", SDL_GetError(), true);
        return 0;
    }

    // Construct Window
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

    Cube cube;

    // Projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 500.0f/500.0f, 0.1f, 10.0f);

    GLint rmLocation = glGetUniformLocation(window.GetShader(), "uProjectionMatrix");
    if (rmLocation < 0) printf("location not found [uProjectionMatrix]");
    else {
        glUniformMatrix4fv(rmLocation, 1, GL_FALSE, &proj[0][0]);
    }


    float yoffset, xoffset, ztranslate, xtranslate;

    // Render Loop
    bool running = true;
    Uint64 frameStart;
    while (running) {
        frameStart = SDL_GetTicks64();

        // CLEAR

        glClearColor(0.1f, 1.0f, 0.6f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DRAW

        cube.Display();

        // INPUTS

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Keyboard state
        const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
        if (state[SDL_SCANCODE_UP]) xoffset += 0.01f;
        if (state[SDL_SCANCODE_DOWN]) xoffset -= 0.01f;
        if (state[SDL_SCANCODE_LEFT]) yoffset += 0.01f;
        if (state[SDL_SCANCODE_RIGHT]) yoffset -= 0.01f;

        if (state[SDL_SCANCODE_W]) ztranslate += 0.05f;
        if (state[SDL_SCANCODE_S]) ztranslate -= 0.05f;
        if (state[SDL_SCANCODE_A]) xtranslate += 0.05f;
        if (state[SDL_SCANCODE_D]) xtranslate -= 0.05f;

        // MOVE

        cube.Rotate(0, float(yoffset*M_PI));
        cube.Move({xtranslate, 0, ztranslate});

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