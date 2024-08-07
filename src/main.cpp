#define SDL_MAIN_HANDLED
#include <random>
#include <memory>

#include <SDL.h>
#include <glew.h>

#include "Window.h"
#include "Player/Player.h"
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

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD); // this is default
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
     * OBJECT MODELS AND TEXTURE LOADING
     */

    // LOAD BLOCK VAOs
    blockVAOmanager = std::make_unique<BlockVAOs>();
    blockVAOmanager->BindBlockModels();

    // LOAD TEXTURES

    // Create the texture manager
    textureManager = std::make_unique<TextureManager>();

    /*
     *  WORLD CREATION
     */

    // Create world
    world = std::make_unique<World>();
    world->GenerateWorld();

    /*
     * PLAYER CREATION
     */

    printf("SPAWN POS %f %f %f\n", 0.5f, world->GenerateBlockHeight({0,0}), 0.5f);

    glm::vec3 startPos = {0.5f, world->GenerateBlockHeight({0,0}) + 3, 0.5f};
    Player player {startPos, {0,0,1.0f}};

    // Set skybox dimensions with player camera
    world->SetSkyboxProperties(player);

    /*
     * LOOP SETUP
     */

    // Trap mouse to screen and hide it
    SDL_SetWindowGrab(window.WindowPtr(), SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);

    // Render Loop
    bool running = true;
    bool escToggled = false;
    SDL_bool grabMouse = SDL_TRUE;
    Uint64 deltaTicks, endTick = SDL_GetTicks64();
    glm::mat4 lastViewMatrix {};

    while (running) {
        /*
         * START OF FRAME
         */

        Uint64 startTick = SDL_GetTicks64();
        deltaTicks = startTick - endTick;
        if (deltaTicks < 1000 / 120) {
            SDL_Delay(Uint32((1000 / 120) - deltaTicks));
        }
        endTick = startTick;

        /*
         *  CLEAR SCREEN
         */

        glClearColor(159/255.0f, 219/255.0f, 245/255.0f, 1.0f); //background colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
         * CHECK OBJECT CULLING
         */

        // If the player has changed the direction they are looking in since last check
        if (player.CameraMoved()) {
            player.GetUsingCamera()->UpdateViewFrustrum();
//            world->CheckCulling(*player.GetUsingCamera());
        }


        /*
         * DRAW TO SCREEN
         */

        world->Display();

        // 2D OVERLAY

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
            player.HandleMovement(deltaTicks);
            player.MouseLook(grabMouse);
        }

//        if (state[SDL_SCANCODE_C]) {
//            secondaryCamera.MoveTo(camera.GetPosition());
//            secondaryCamera.SetDirection(camera.GetDirection());
//            printf("campos %f %f %f\n", secondaryCamera.GetPosition().x, secondaryCamera.GetPosition().y, secondaryCamera.GetPosition().z);
//        }
//
//        if (state[SDL_SCANCODE_1]) curCam = &camera;
//        if (state[SDL_SCANCODE_2]) curCam = &secondaryCamera;

        if (state[SDL_SCANCODE_F]) printf("FPS: %llu\n", 1000/(deltaTicks + 1));

        /*
         *  UDPATE OBJECTS
         */

        world->SetSkyboxPosition(player.GetPosition());

        /*
         * UPDATE DISPLAY
         */

        player.GetUsingCamera()->UpdateLookatUniform();
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