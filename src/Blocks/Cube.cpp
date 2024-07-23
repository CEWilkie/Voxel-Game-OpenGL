//
// Created by cew05 on 07/07/2024.
//

#include "Cube.h"

#include "../Window.h"
#include <SDL.h>
#include <SDL_image.h>

Cube::Cube() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &indexBufferObject);

    // Get location for modelMatrix
    modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uModelMatrix");
    if (modelMatrixLocation < 0) printf("location not found [uModelMatrix]");

    // [POSITION], [TEXCOORD], both values are offsets relative to the set origin points
    std::vector<Vertex> vertexVectors = {
            // Front
            { glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 1.0f) },            // TOPLEFT VERTEX
            { glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(2.0f, 1.0f) },            // TOPRIGHT VERTEX
            { glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 2.0f) },            // BOTTOMLEFT VERTEX
            { glm::vec3(0.0f, -1.0f, 1.0f), glm::vec2(2.0f, 2.0f) },            // BOTTOMRIGHT VERTEX

            // Left
            { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },            // TOPLEFT VERTEX
            { glm::vec3(1.0f, -1.0f, 0.0f),glm::vec2(0.0f, 2.0f) },            // BOTTOMLEFT VERTEX

            // Right
            { glm::vec3(1.0f, 0.0f, 1.0f),  glm::vec2(3.0f, 1.0f) },            // TOPRIGHT VERTEX
            { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(3.0f, 2.0f) },            // BOTTOMRIGHT VERTEX

            // Back
            { glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(4.0f, 1.0f) },             // TOPRIGHT VERTEX
            { glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec2(4.0f, 2.0f) },             // BOTTOMRIGHT VERTEX

            // Top
            { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },            // TOPLEFT VERTEX
            { glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(2.0f, 0.0f) },            // TOPRIGHT VERTEX

            // Bottom
            { glm::vec3(1.0f, -1.0f, 0.0f),glm::vec2(1.0f, 3.0f) },            // BACKLEFT VERTEX
            { glm::vec3(1.0f, -1.0f, 1.0f),glm::vec2(2.0f, 3.0f) },            // BACKRIGHT VERTEX

    };

    // Populate vertexArray with this data
    vertexArray = std::make_unique<std::vector<Vertex>>(vertexVectors);

    // Set texture origin position
    textureOrigin = {0.0f, 0.0f};

    // Create transformation object
    transformation = std::make_unique<Transformation>();

    // Create index buffer for traversal order to produce each cube face
    indexArray = {
            11, 10, 0, 1, 11, 0,
            1, 0, 3, 2, 3, 0,
            1, 3, 7, 6, 1, 7,
            7, 9, 6, 8, 6, 9,
            5, 2, 4, 0, 4, 2,
            2, 12, 3, 13, 3, 12
    };

    // Create bounding models for culling purposes
    auto sb = GenerateSphere(*vertexArray);
    sphereBounds = new SphereBounds(sb.centre, sb.radius);

    BindCube();
}

Cube::~Cube() {
    glDeleteBuffers(1, &vertexBufferObject);
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

    // Vertex TextureData attributes
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
    // Can display?
    if (!canDisplay) return;

    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Update model matrix to uniform
    if (modelMatrixLocation >= 0) glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &transformation->GetModelMatrix()[0][0]);

    // Activate texture
    if (texture != nullptr) texture->EnableTexture();

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // unbind
    if (texture != nullptr) texture->DisableTexture();
    glBindVertexArray(0);
}

bool Cube::CheckCulling(const Camera& _camera) {
    // Check sphere bounds
    canDisplay = sphereBounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation);
    return canDisplay;
}

void Cube::UpdateTextureData() {
    for (auto& vertex : *vertexArray ) {
        vertex.texture = texture->GetTextureSheetTile(textureOrigin + vertex.textureIndex);
    }

    // Update buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::SetTexture(TextureData* _texture, glm::vec2 _origin) {
    if (_texture == nullptr) {
        printf("TextureData cannot be assigned to null\n");
        return;
    }

    // WARNING ENSURE CHECK FOR NULL TEXTURE BEFORE USING IN DISPLAY
    texture = _texture;

    // Set offset of texture position in texturesheet. non-sheets will use (0.0f, 0.0f).
    textureOrigin = _origin;

    // Update stored texture data
    UpdateTextureData();
}

void Cube::SetTextureOrigin(glm::vec2 _origin) {
    // Set offset of texture position in texturesheet. non-sheets will use (0.0f, 0.0f).
    textureOrigin = _origin;

    // Update stored texture data
    UpdateTextureData();
}

void Cube::SetPositionOrigin(glm::vec3 _originPosition) {
    // set transformation to move to the new origin position
    transformation->SetPosition(_originPosition);
}

void Cube::SetPositionCentre(glm::vec3 _centre) {
    // set transformation to move to the new origin position, but offset by half the scale of the cube
    transformation->SetPosition(_centre - (transformation->GetLocalScale()/2.0f));
}

void Cube::SetScale(glm::vec3 _scale) {
    transformation->SetScale(_scale);
}

void Cube::SetRotation(glm::vec3 _rotation) {
    transformation->SetRotation(_rotation);
}

void Cube::UpdateModelMatrix() {
    transformation->UpdateModelMatrix();
}