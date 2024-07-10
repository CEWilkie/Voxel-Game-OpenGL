//
// Created by cew05 on 07/07/2024.
//

#include "Cube.h"

#include "Window.h"
#include <SDL.h>
#include <SDL_image.h>

Cube::Cube() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &colorBufferObject);
    glGenBuffers(1, &indexBufferObject);
    glGenBuffers(1, &textureBufferObject);

    // Create vertex array for cube
    vertexArray = {
            // Top level
            0.0f, 0.0f, 0.0f,               // BOTTOMLEFT VERTEX
            0.5f, 0.0f, 0.0f,               // BOTTOMRIGHT VERTEX
            0.0f, 0.0f, 0.5f,               // TOPLEFT VERTEX
            0.5f, 0.0f, 0.5f,               // TOPRIGHT VERTEX
            // Bottom level
            0.0f, -0.5f, 0.0f,               // BOTTOMLEFT VERTEX
            0.5f, -0.5f, 0.0f,               // BOTTOMRIGHT VERTEX
            0.0f, -0.5f, 0.5f,               // TOPLEFT VERTEX
            0.5f, -0.5f, 0.5f,               // TOPRIGHT VERTEX
    };

    // Create index buffer for traversal order to produce each cube face
    indexArray = {
            // TOPFACE
            0, 1, 2,
            1, 3, 2,
            // FRONTFACE
            4, 5, 0,
            5, 1, 0,
            // RIGHTSIDEFACE
            5, 7, 1,
            7, 3, 1,
            // LEFTSIDEFACE
            6, 4, 2,
            4, 0, 2,
            // BOTTOMFACE
            4, 5, 6,
            5, 7, 6,
            // BACKFACE
            7, 2, 3,
            7, 6, 2,
    };

    // Create colour array
    vertexColorArray = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    // Create vertex texture points
    vertexTextureArray = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    CreateTextures();

    BindCube();
}

Cube::~Cube() {
    printf("Cube Destroyed\n");

    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &colorBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}


void Cube::BindCube() {
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STATIC_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*3, nullptr);

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data(), GL_STATIC_DRAW);

    // Bind colour buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexColorArray.size() * sizeof(float)), vertexColorArray.data(), GL_STATIC_DRAW);

    // Vertex Colour Attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*3, nullptr);

    // Bind texture buffer
    glBindBuffer(GL_TEXTURE_2D, textureBufferObject);
    glBufferData(GL_TEXTURE_2D, GLsizeiptr(vertexTextureArray.size() * sizeof(float)), vertexTextureArray.data(), GL_STATIC_DRAW);

    // Vertex Texture attributes
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*2, nullptr);

    GLint tex0Location = glGetUniformLocation(window.GetShader(), "tex0");
    glUniform1i(tex0Location, 0);

    // Unbind arrays / buffers
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_2D, 0);

    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR) {
        printf("GL ERROR CODE %u STRING : %s\n", e, glewGetErrorString(e));
    }
}


void Cube::Display() const {
    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Enable Attributes
    glEnable(GL_DEPTH_TEST);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // unbind
    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Cube::CreateTextures() const {
    // Load image to surface
    SDL_Surface* surface = IMG_Load("../resources/testcube64x.png");
    if (surface == nullptr) {
        LogError("Failed to load texture", SDL_GetError());
        return;
    }

    // Format RGBA
    Uint8 nColours = surface->format->BytesPerPixel;
    GLenum textureFormat;
    GLint internalFormat;
    if (nColours == 4) { // Contains alpha
        if (surface->format->Rmask == 0x000000ff) textureFormat = GL_RGBA;
        else textureFormat = GL_BGRA;
        internalFormat = GL_RGBA8;
    }
    else if (nColours == 3) { // No alpha
        if (surface->format->Rmask == 0x000000ff) textureFormat = GL_RGB;
        else textureFormat = GL_BGR;
        internalFormat = GL_RGB8;
    }
    else {
        printf("Image potentially unsuitable, only %d colour channels.\n Ending texture assignment\n", nColours);
        return;
    }

    // Pixel Alignment Info
    int alignment = 8;
    while (surface->pitch%alignment) alignment>>=1; // x%1==0 for any x
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);

    // bind to the texture object
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureBufferObject);

    // Set texture stretch properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // Texture environment interactions
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0,
                 textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(surface);
}


void Cube::SetPosition(const std::vector<float> &_originVertex) {
    vertexArray = {
            // Top level
            _originVertex[0]        , _originVertex[1]          , _originVertex[2]          ,               // BOTTOMLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1]          , _originVertex[2]          ,               // BOTTOMRIGHT VERTEX
            _originVertex[0]        , _originVertex[1]          , _originVertex[2] + 0.5f   ,               // TOPLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1]          , _originVertex[2] + 0.5f   ,               // TOPRIGHT VERTEX
            // Bottom level
            _originVertex[0]        , _originVertex[1] - 0.5f   , _originVertex[2]          ,               // BOTTOMLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1] - 0.5f   , _originVertex[2]          ,               // BOTTOMRIGHT VERTEX
            _originVertex[0]        , _originVertex[1] - 0.5f   , _originVertex[2] + 0.5f   ,               // TOPLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1] - 0.5f   , _originVertex[2] + 0.5f   ,               // TOPRIGHT VERTEX
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

