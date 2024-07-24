#define SDL_MAIN_HANDLED
#include <random>
#include <memory>

#include <SDL.h>
#include <glew.h>

#include "Window.h"
#include "Player/Camera.h"
#include "World/World.h"
#include "Textures/TextureManager.h"

#include "World/Chunk.h"

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


    // CREATE CAMERA
    Camera camera;
    Camera secondaryCamera;
    Camera* curCam = &camera;

    // LOAD TEXTURES

    // Create the texture manager
    textureManager = std::make_unique<TextureManager>();

    /*
     *  WORLD CREATION
     */

    // Create world
    world = std::make_unique<World>();
    world->SetSkyboxProperties(&camera);
    world->GenerateWorld();



    // Trap mouse to screen and hide it
    SDL_SetWindowGrab(window.WindowPtr(), SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    // Render Loop
    bool running = true;
    bool escToggled = false;
    SDL_bool grabMouse = SDL_TRUE;
    Uint64 deltaFrames, lastFrame = SDL_GetTicks64();
    glm::mat4 lastViewMatrix {};
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
         * CHECK OBJECT CULLING
         */

        // If view matrix has changed since last check
        if (lastViewMatrix != camera.GetViewMatrix()) {
            lastViewMatrix = camera.GetViewMatrix();
            camera.UpdateViewFrustrum();

            //chunk.CheckCulling(camera);
        }


        /*
         * DRAW TO SCREEN
         */

        world->Display();

        // 2D OVERLAY
        glDisable(GL_DEPTH_TEST);

        // positive x,y,z directions
        camera.DisplayViewBounds();

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

        if (state[SDL_SCANCODE_C]) {
            secondaryCamera.MoveTo(camera.GetPosition());
            secondaryCamera.SetDirection(camera.GetDirection());
        }

        if (state[SDL_SCANCODE_1]) curCam = &camera;
        if (state[SDL_SCANCODE_2]) curCam = &secondaryCamera;

        /*
         *  UDPATE OBJECTS
         */

        world->SetSkyboxPosition(camera.GetPosition());

        /*
         * UPDATE DISPLAY
         */

        curCam->UpdateUniform();
        SDL_GL_SwapWindow(window.WindowPtr());

        /*
         *  END OF FRAME
         */



    }

    /*
     * HANDLE END OF PROGRAM
     */

    SDL_Quit();

    return 0;
}