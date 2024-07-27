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

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD); // this is default
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    // Create bounding box for world objects (transformed to necessary size when used)
    blockBounds = std::make_unique<BoxBounds>(
            GenerateBoxBounds({{{0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f, 1.0f}}}));

    // Create world
    world = std::make_unique<World>();
    world->SetSkyboxProperties(&camera);
    world->GenerateWorld();

    // Trap mouse to screen and hide it
    SDL_SetWindowGrab(window.WindowPtr(), SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);


    GLuint vertexArrayObject, vertexBufferObject, indexBufferObject;

    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &indexBufferObject);

    // Create vertex positions (two triangles
    std::vector<Vertex> vertexArray = {
            // bottom Left Triangle
            { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
            { glm::vec3(50.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            { glm::vec3(0.0f, 50.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            { glm::vec3(50.0f, 50.0f, 0.0f), glm::vec2(01.0f, 0.0f)},
    };

    std::vector<GLuint> indexArray {
        0, 1, 2, 1, 3, 2
    };

    // bind object id
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(Vertex)), vertexArray.data(), GL_STREAM_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, position));

    // Vertex Colour Attributes
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, color));

    // Vertex TextureData attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, textureCoord));

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data(), GL_STATIC_DRAW);

    GLint tex0Location = glGetUniformLocation(window.GetShader(), "tex0");
    glUniform1i(tex0Location, 0);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Transformation t {};
    t.SetScale({0.25f, 0.25f, 0.25f});

    // vertical rotation
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),
                           (float)glm::radians(45.0f),
                           glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

    glm::vec3 direction = glm::normalize(rotation * glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f));

    t.SetRotation(direction);
    t.SetPosition({0, 100, 50});
    t.UpdateModelMatrix();

    t.SetRotation(direction);
    t.UpdateModelMatrix();

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

           // world->CheckCulling(camera);
        }


        /*
         * DRAW TO SCREEN
         */

        world->Display();

        // Bind object
        glBindVertexArray(vertexArrayObject);

        // Update model matrix to uniform
        GLint modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uModelMatrix");
        if (modelMatrixLocation < 0) printf("sun location not found [uModelMatrix]\n");
        if (modelMatrixLocation >= 0) glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &t.GetModelMatrix()[0][0]);

        modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uVertexTextureCoordOffset");
        if (modelMatrixLocation < 0) printf("sun location not found [uVertexTextureCoordOffset]\n");
        if (modelMatrixLocation >= 0) glUniform2fv(modelMatrixLocation, 1, nullptr);

        textureManager->EnableTextureSheet(TEXTURESHEET::WORLD);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        // 2D OVERLAY
        glDisable(GL_DEPTH_TEST);

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

        if (state[SDL_SCANCODE_F]) printf("FPS: %llu\n", 1000/(deltaFrames+1));

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