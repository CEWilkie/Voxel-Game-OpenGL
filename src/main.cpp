#define SDL_MAIN_HANDLED
#include <random>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>

#include <glew.h>

#include "Window.h"
#include "Cube.h"
#include "Camera.h"

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
    for (int c = 0; c < 20; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({float(c / 2.0), 0.f, 0.0f});
        cubes.push_back(std::move(cube));
    }


    // Line of cubes in y
    for (int c = 0; c < 20; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({0.0f, float(c / 2.0), 0.0f});
        cubes.push_back(std::move(cube));
    }


    // Line of cubes in z
    for (int c = 0; c < 10; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({0.0f, 0.f, float(c / 2.0)});
        cubes.push_back(std::move(cube));
    }

//    return 0;

    // CAMERA OBJECT
    Camera camera;


    double yrotate = 0, xrotate = 0;
    float ztranslate = 0, xtranslate = 0;

    // Trap mouse to screen and hide it
    SDL_SetWindowGrab(window.WindowPtr(), SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    // Render Loop
    bool running = true;
    bool escToggled = false;
    SDL_bool grabMouse = SDL_TRUE;
    Uint64 deltaFrames, lastFrame;
    while (running) {
        /*
         * START OF FRAME
         */

        Uint64 frameStart = SDL_GetTicks64();
        deltaFrames = frameStart - lastFrame;
        lastFrame = frameStart;

        /*
         *  CLEAR SCREEN
         */

        glClearColor(159/255.0f, 219/255.0f, 245/255.0f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
         * DRAW TO SCREEN
         */

        glClearErrors();

        for (const auto& cube : cubes) {
            cube->Display();
        }

        camera.DisplayDirectionVertexes();

        /*
         * INPUT MANAGEMENT
         */

        // SDL EVENTS

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // KEYBOARD INPUTS

        // Mouse position -> Looking target
        const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
        if (state[SDL_SCANCODE_ESCAPE]) {
            // Guard against escape being set to false after 2 frame press
            if (escToggled) continue;
            escToggled = true;

            // Set mouse pos to centre to prevent change in looking target on resume
            int maxx, maxy;
            window.GetWindowSize(maxx, maxy);
            SDL_WarpMouseInWindow(window.WindowPtr(), maxx/2, maxy/2);

            // lock mouse to window
            grabMouse = (grabMouse == SDL_TRUE) ? SDL_FALSE : SDL_TRUE;
            SDL_SetWindowGrab(window.WindowPtr(), grabMouse);
            SDL_ShowCursor((grabMouse == SDL_TRUE) ? SDL_DISABLE : SDL_ENABLE);
        }
        if (!state[SDL_SCANCODE_ESCAPE]) escToggled = false;

        // CAMERA
        if (grabMouse == SDL_TRUE) {
            camera.Move(deltaFrames);
            camera.MouseLook(grabMouse);
        }

        /*
         * ERROR CHECKING
         */

        /*
         *  APPLY INPUTS
         */



        /*
         * UPDATE DISPLAY
         */

        camera.UpdateUniform();
        SDL_GL_SwapWindow(window.WindowPtr());

        /*
         *  END OF FRAME
         */

//        Uint64 frametime = SDL_GetTicks64() - frameStart;
//        if (frametime < 1000/60) {
//            SDL_Delay(1000/60 - frametime);
//        }

    }

    /*
     * HANDLE END OF PROGRAM
     */

    SDL_Quit();

    return 0;
}