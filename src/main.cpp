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

    std::vector<std::unique_ptr<Cube>> cubes;

    // Line of cubes in x
    for (int c = -10; c < 10; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPosition({float(1+c), 0.f, 0.f});
        cubes.push_back(std::move(cube));
    }

    // Line of cubes in y
    for (int c = -10; c < 10; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPosition({1.f, float(1+c), 0.f});
        cubes.push_back(std::move(cube));
    }

    // Line of cubes in z
    for (int c = -10; c < 10; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPosition({1.f, 0.f, float(1+c)});
        cubes.push_back(std::move(cube));
    }

    // Projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), window.GetAspectRatio(), 0.1f, 10.0f);

    GLint rmLocation = glGetUniformLocation(window.GetShader(), "uProjectionMatrix");
    if (rmLocation < 0) printf("location not found [uProjectionMatrix]");
    else {
        glUniformMatrix4fv(rmLocation, 1, GL_FALSE, &proj[0][0]);
    }


    double yrotate, xrotate;
    float ztranslate, xtranslate;

    // Get mouse position to use for looking direction input
    int mousex, mousey, lastmousex, lastmousey;
    SDL_GetMouseState(&lastmousex, &lastmousey);

    // Render Loop
    bool running = true;
    Uint64 frameStart;
    while (running) {
        frameStart = SDL_GetTicks64();

        // CLEAR

        glClearColor(0.0f, 0.2f, 0.2f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DRAW

        for (const auto& cube : cubes) {
            cube->Display();
        }

        // INPUTS

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Mouse position -> Looking direction
        int maxx, maxy;
        window.GetWindowSize(maxx, maxy);
        SDL_GetMouseState(&mousex, &mousey);

        if (mousex <= maxx && mousex >= 0 && mousey <= maxy && mousey >= 0){
            if (lastmousex != mousex || lastmousey != mousey) {
                yrotate += ((lastmousex - mousex) / (maxx/2.0)) * M_PI * 0.5f;
                xrotate -= ((lastmousey - mousey) / (maxy/2.0)) * M_PI * 0.5f;
                lastmousex = mousex;
                lastmousey = mousey;
            }
        }

        // Ensure min/max lookup/down
        xrotate = std::max(-1.0, xrotate);
        xrotate = std::min(1.0, xrotate);

        // Keyboard state
        const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
        if (state[SDL_SCANCODE_UP]) xrotate -= 0.01f * M_PI;
        if (state[SDL_SCANCODE_DOWN]) xrotate += 0.01f * M_PI;
        if (state[SDL_SCANCODE_LEFT]) yrotate += 0.02f * M_PI;
        if (state[SDL_SCANCODE_RIGHT]) yrotate -= 0.02f * M_PI;

        if (state[SDL_SCANCODE_W]) {
            ztranslate += 0.05f * (float)cos(yrotate);
            xtranslate += 0.05f * (float)sin(yrotate);
        }
        if (state[SDL_SCANCODE_S]) {
            ztranslate -= 0.05f * (float)cos(yrotate);
            xtranslate -= 0.05f * (float)sin(yrotate);
        }
        if (state[SDL_SCANCODE_A]) {
            ztranslate -= 0.05f * (float)sin(yrotate);
            xtranslate += 0.05f * (float)cos(yrotate);
        }
        if (state[SDL_SCANCODE_D]) {
            ztranslate += 0.05f * (float)sin(yrotate);
            xtranslate -= 0.05f * (float)cos(yrotate);
        }

        // modify translation according to facing direction;
        printf("FACING ANGLE %f %f\n", (float)yrotate, (float)xrotate);

        // Error check
        GLenum e;
        while ((e = glGetError()) != GL_NO_ERROR) {
            printf("GL ERROR CODE %u STRING : %s\n", e, glewGetErrorString(e));
        }

        // MOVE

        for (const auto& cube : cubes) {
            cube->Rotate({float(xrotate), float(yrotate), 0});
            cube->Move({xtranslate, 0, ztranslate});
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