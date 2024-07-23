#define SDL_MAIN_HANDLED
#include <random>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>

#include <glew.h>

#include "Window.h"
#include "Cube.h"
#include "Camera.h"
#include "World.h"

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


    /*
     * TEXTURE CREATION
     */

    Texture texture("../resources/testcube64x.png");
    texture.SetTextureSheetGrid({4, 4});
    Texture textureB("../resources/testcubes16x.png");
    textureB.SetTextureSheetGrid({16, 16});

    /*
     *  WORLD OBJECT CREATION
     */

    // CAMERA OBJECT
    Camera camera;

    // WORLD OBJECT
    World world;
    world.SetSkyboxProperties(&camera);

    // CUBES

    std::vector<std::unique_ptr<Cube>> cubes;

    // Line of cubes in x
    for (int c = 0; c < 20; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({float(c), 0.f, 0.0f});
        cube->SetTexture(&texture, {0,0});
        cube->UpdateModelMatrix();
        cubes.push_back(std::move(cube));
    }


    // Line of cubes in y
    for (int c = 0; c < 20; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({0.0f, float(c), 0.0f});
        cube->SetTexture(&texture, {0,0});
        cube->UpdateModelMatrix();
        cubes.push_back(std::move(cube));
    }


    // Line of cubes in z
    for (int c = 0; c < 10; c++) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionOrigin({0.0f, 0.f, float(c)});
        cube->SetTexture(&texture, {0,0});
        cube->UpdateModelMatrix();
        cubes.push_back(std::move(cube));
    }

    std::vector<std::unique_ptr<Cube>> fc;
    // INDEX SIDE : |0 LEFT | 1 RIGHT | 2 BOTTOM | 3 TOP | 4 NEAR | 5 FAR
    glm::vec2 origins[6]{{0, 0}, {3, 1}, {4, 3}, {6, 0}, {9, 1}, {12, 0}};
    int o = 0;
    for (const auto& plane : camera.GetCameraFrustrum().planes) {
        std::unique_ptr<Cube> cube = std::make_unique<Cube>();
        cube->SetPositionCentre(plane.normal);
        cube->SetTexture(&textureB, origins[o]);
        cube->SetScale({0.1f, 1.0f, 0.1f});
        cube->UpdateModelMatrix();
        fc.push_back(std::move(cube));
        o++;
    }


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

            unsigned int t = cubes.size();
            for (const auto& cube : cubes) {
                if (!cube->CheckCulling(camera)) {
                    // Cube is culled, remove from displaying list
                    t--;
                }
            }

            printf("OF %zu CUBES, %u RENDERED\n", cubes.size(), t);
        }


        /*
         * DRAW TO SCREEN
         */

        // 3D OBJECTS
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Display the block objects
        for (const auto& cube : cubes) {
            cube->Display();
        }

        // Display the block objects
        for (const auto& cube : fc) {
            cube->Display();
        }

        glDisable(GL_CULL_FACE);

        // Display the background images
        world.Display();

        // 2D OVERLAY
        glDisable(GL_DEPTH_TEST);

        // positive x,y,z directions
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
         *  UDPATE OBJECTS
         */

        world.SetSkyboxPosition(camera.GetPosition());

        /*
         * UPDATE DISPLAY
         */

        camera.UpdateUniform();
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