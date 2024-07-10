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
    glGenBuffers(1, &indexBufferObject);
    glGenBuffers(1, &textureBufferObject);

    // [POSITION], [TEXCOORD], both values are relative to the set origin points
    std::vector<VertexOffsets> vertexVectors = {
            // Front
            { glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec2(0.25f, 0.25f) },            // TOPLEFT VERTEX
            { glm::vec3(0.0f, 0.0f, 0.5f),  glm::vec2(0.5f, 0.25f) },            // TOPRIGHT VERTEX
            { glm::vec3(0.0f, -0.5f, 0.0f), glm::vec2(0.25f, 0.5f) },            // BOTTOMLEFT VERTEX
            { glm::vec3(0.0f, -0.5f, 0.5f), glm::vec2(0.5f, 0.5f) },            // BOTTOMRIGHT VERTEX

            // Left
            { glm::vec3(0.5f, 0.0f, 0.0f), glm::vec2(0.0f, 0.25f) },            // TOPLEFT VERTEX
            { glm::vec3(0.5f, -0.5f, 0.0f),glm::vec2(0.0f, 0.5f) },            // BOTTOMLEFT VERTEX

            // Right
            { glm::vec3(0.5f, 0.0f, 0.5f),  glm::vec2(0.75f, 0.25f) },            // TOPRIGHT VERTEX
            { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.75f, 0.5f) },            // BOTTOMRIGHT VERTEX

            // Back
            { glm::vec3(0.5f, 0.0f, 0.0f),  glm::vec2(1.0f, 0.25f) },             // TOPRIGHT VERTEX
            { glm::vec3(0.5f, -0.5f, 0.0f),  glm::vec2(1.0f, 0.5f) },             // BOTTOMRIGHT VERTEX

            // Top
            { glm::vec3(0.5f, 0.0f, 0.0f), glm::vec2(0.25f, 0.0f) },            // TOPLEFT VERTEX
            { glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(0.5f, 0.0f) },            // TOPRIGHT VERTEX

            // Bottom
            { glm::vec3(0.5f, -0.5f, 0.0f),glm::vec2(0.25f, 0.75f) },            // BACKLEFT VERTEX
            { glm::vec3(0.5f, -0.5f, 0.5f),glm::vec2(0.5f, 0.75f) },            // BACKRIGHT VERTEX

    };

    // Populate vertexOffsetArray with data
    vertexOffsetArray = std::make_unique<std::vector<VertexOffsets>>(vertexVectors);

    // Set default origin position
    origin = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f),
              glm::vec3(1.0f, 1.0f, 1.0f)};

    // Create vertexArray of default Vertex Positions
    vertexArray = std::make_unique<std::vector<Vertex>>();
    for (const auto& vertexVector : vertexVectors) {
        Vertex v;
        v.position = vertexVector.positionOffset;
        v.texture = vertexVector.textureOffset;
        vertexArray->push_back(v);
    }

    // Create index buffer for traversal order to produce each cube face
    indexArray = {
            10, 11, 0, 1, 0, 11,
            0, 1, 3, 2, 0, 3,
            1, 6, 3, 7, 3, 6,
            7, 9, 6, 8, 6, 9,
            5, 2, 4, 0, 4, 2,
            2, 12, 3, 13, 3, 12
    };

    CreateTextures();

    BindCube();
}

Cube::~Cube() {
    Vertex v;
    glGetBufferSubData(GL_ARRAY_BUFFER, vertexBufferObject, GLsizeiptr(sizeof(struct Vertex)), &v);
    printf("Cube Destroyed at %f %f %f\n",
           v.position.x, v.position.y, v.position.z);

    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &textureBufferObject);
    glDeleteBuffers(1, &indexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}


void Cube::BindCube() const {
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data(), GL_STATIC_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, position));

    // Vertex Colour Attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, color));

    // Vertex Texture attributes
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, texture));

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data(), GL_STATIC_DRAW);

    GLint tex0Location = glGetUniformLocation(window.GetShader(), "tex0");
    glUniform1i(tex0Location, 0);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // unbind
    glDisable(GL_DEPTH_TEST);
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


void Cube::SetPositionOrigin(glm::vec3 _origin) {
    origin.position =_origin;

    // Update the vertex positions
    UpdateVertexPositions();
}

void Cube::SetTextureOrigin(glm::vec2 _origin) {
    origin.texture = _origin;

    // Update the texture positions
    UpdateVertexTextureCoords();
}

void Cube::UpdateVertexPositions() const {
    // Update vertex position data
    for (int v = 0; v < vertexArray->size(); v++) {
        vertexArray->at(v).position = origin.position + vertexOffsetArray->at(v).positionOffset;

        if (vertexArray->at(v).position == glm::vec3(0.0f, 0.0f, 0.0f))
            vertexArray->at(v).color = glm::vec3(1.0f, 1.0f, 1.0f);

        if (origin.position.z > 0) vertexArray->at(v).color = glm::vec3(0.0f, 1.0f, 0.0f);
        if (origin.position.x > 0) vertexArray->at(v).color = glm::vec3(0.0f, 0.0f, 1.0f);
        if (origin.position.y > 0) vertexArray->at(v).color = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Bind data to buffer. Vector can now be dropped as it is stored in buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::UpdateColorBuffer() const {
    // Set vertex with new colours
    // ...

    // Bind data to buffer. Vector can now be dropped as it is stored in buffer.
    glBindBuffer(GL_TEXTURE_2D, textureBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::UpdateVertexTextureCoords() const {
    std::vector<float> data {};

    // Update vector texture coordinates with new correct value
    for (int v = 0; v < vertexArray->size(); v++) {
        vertexArray->at(v).texture = origin.texture + vertexOffsetArray->at(v).textureOffset;
    }

    // Bind data to buffer. Vector can now be dropped as it is stored in buffer.
    glBindBuffer(GL_TEXTURE_2D, textureBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}