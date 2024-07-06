#define SDL_MAIN_HANDLED
#include <iostream>
#include <random>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>

#include "Window.h"
#include "Triangle.h"

struct TriangleObject {
    unsigned int vertexArrayObject {};
    unsigned int vertexBufferObject {};
    std::vector<float> vertexArray {};
};




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

    std::vector<TriangleObject> tov;

    std::vector<std::unique_ptr<Triangle>> triangles;

    for (int t = 0; t < 2; t++) {
        std::vector<float> v {};
        tov.emplace_back();
        triangles.push_back(std::make_unique<Triangle>());
        for (int i = 0; i < 9; i++) {
            auto x = float((int(mt()) % 100) / 100.0);
            tov.back().vertexArray.push_back(x);
            v.push_back(x);
        }
        triangles[t]->ConstructTriangle(3, v);
    }

    printf("TRIANGLES: %zu\n", triangles.size());

    for (auto& to : tov) {
        // bind object id
        glGenVertexArrays(1, &to.vertexArrayObject);
        glBindVertexArray(to.vertexArrayObject);
        printf("VAO: %u", to.vertexArrayObject);

        // bind vertexArray array
        glGenBuffers(1, &to.vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, to.vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(to.vertexArray.size() * sizeof(float)), to.vertexArray.data(), GL_DYNAMIC_DRAW);
        printf(" VBO: %u\n", to.vertexBufferObject);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    // Render Loop
    bool running = true;

    Uint64 frameStart;
    while (running) {
        frameStart = SDL_GetTicks64();

        // CLEAR

        glClearColor(0.1f, 1.0f, 0.6f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT);

        // DRAW
//        for (const auto& to : tov) {
//            glBindVertexArray(to.vertexArrayObject);
//            glBindBuffer(GL_ARRAY_BUFFER, to.vertexBufferObject);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//        }

        for (const auto& t : triangles) {
            t->Display();
        }
        // MOVE

        for (auto& to : tov) {
            glBindBuffer(GL_ARRAY_BUFFER, to.vertexBufferObject);
            for (auto& vertex : to.vertexArray) {
                vertex += 0.005f;
            }
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(to.vertexArray.size() * sizeof(float)), to.vertexArray.data(), GL_DYNAMIC_DRAW);
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
    window.EndWindow();
    SDL_Quit();

    return 0;
}