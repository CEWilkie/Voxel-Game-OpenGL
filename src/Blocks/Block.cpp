//
// Created by cew05 on 07/07/2024.
//

#include "Block.h"

#include "../Window.h"
#include <SDL.h>
#include <SDL_image.h>

Block::Block() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &indexBufferObject);

    BindCube();
}

Block::~Block() {
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &indexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}


std::vector<Vertex> Block::BaseVertexArray() {
    // [POSITION], [TEXCOORD], both values are offsets relative to the set origin points
    return {
            // Front
            { glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 0.0f) },            // TOPLEFT VERTEX
            { glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 0.0f) },            // TOPRIGHT VERTEX
            { glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },            // BOTTOMLEFT VERTEX
            { glm::vec3(0.0f, -1.0f, 1.0f), glm::vec2(1.0f, 1.0f) },            // BOTTOMRIGHT VERTEX

            // Left
            { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(-1.0f, 0.0f) },            // TOPLEFT VERTEX
            { glm::vec3(1.0f, -1.0f, 0.0f),glm::vec2(-1.0f, 1.0f) },            // BOTTOMLEFT VERTEX

            // Right
            { glm::vec3(1.0f, 0.0f, 1.0f),  glm::vec2(2.0f, 0.0f) },            // TOPRIGHT VERTEX
            { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(2.0f, 1.0f) },            // BOTTOMRIGHT VERTEX

            // Back
            { glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(3.0f, 0.0f) },             // TOPRIGHT VERTEX
            { glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec2(3.0f, 1.0f) },             // BOTTOMRIGHT VERTEX

            // Top
            { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, -1.0f) },            // TOPLEFT VERTEX
            { glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, -1.0f) },            // TOPRIGHT VERTEX

            // Bottom
            { glm::vec3(1.0f, -1.0f, 0.0f),glm::vec2(0.0f, 2.0f) },            // BACKLEFT VERTEX
            { glm::vec3(1.0f, -1.0f, 1.0f),glm::vec2(1.0f, 2.0f) },            // BACKRIGHT VERTEX
    };
}

std::vector<GLuint> Block::BaseIndexArray() {
    return {
            11, 10, 0, 1, 11, 0,
            1, 0, 3, 2, 3, 0,
            1, 3, 7, 6, 1, 7,
            7, 9, 6, 8, 6, 9,
            5, 2, 4, 0, 4, 2,
            2, 12, 3, 13, 3, 12
    };
}

void Block::SetBlockData(BlockData _data) {
    blockData = _data;
}


void Block::BindCube() const {
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(BaseVertexArray().size() * sizeof(Vertex)), BaseVertexArray().data(), GL_STATIC_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, position));

    // Vertex Colour Attributes
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, color));

    // Vertex TextureData attributes
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, textureCoord));

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(BaseIndexArray().size()*sizeof(GLuint)), BaseIndexArray().data(), GL_STATIC_DRAW);

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










void Block::Display() const {
    if (isCulled) return;

    if (blockData.blockID == BLOCKID::AIR) return;

    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Update model matrix to uniform
    GLint modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uModelMatrix");
    if (modelMatrixLocation < 0) printf("location not found [uModelMatrix]");
    if (modelMatrixLocation >= 0) glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &transformation->GetModelMatrix()[0][0]);

    textureManager->EnableTextureSheet(blockData.textureSheet);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

bool Block::CheckCulling(const Camera& _camera) {
    isCulled = !blockBounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation);
    return isCulled;
}


std::vector<Vertex> Block::GetTrueTextureCoords(TEXTURESHEET _sheetID, glm::vec2 _textureOrigin) {
    std::vector<Vertex> vertexArray = BaseVertexArray();
    auto textureData = textureManager->GetTextureData(_sheetID);

    for (auto& vertex : vertexArray) {
        vertex.textureCoord = textureData->GetTextureSheetTile(_textureOrigin + vertex.textureCoord);
    }

    return vertexArray;
}

void Block::SetTexture(TEXTURESHEET _textureID, glm::vec2 _origin) {
    // Update stored texture data
    std::vector<Vertex> vertexArray = GetTrueTextureCoords(_textureID, _origin);
    blockData.textureSheet = _textureID;

    // Update buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray.size() * sizeof(struct Vertex)), vertexArray.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Block::SetPositionOrigin(glm::vec3 _originPosition) {
    // set transformation to move to the new origin position
    transformation->SetPosition(_originPosition);
}

void Block::SetPositionCentre(glm::vec3 _centre) {
    // set transformation to move to the new origin position, but offset by half the scale of the cube
    glm::vec3 originFromCentre{_centre - (transformation->GetLocalScale() / 2.0f)};
    originFromCentre.y += transformation->GetLocalScale().y;

    transformation->SetPosition(originFromCentre);
}

void Block::SetScale(glm::vec3 _scale) {
    transformation->SetScale(_scale);
}

void Block::SetRotation(glm::vec3 _rotation) {
    transformation->SetRotation(_rotation);
}

void Block::UpdateModelMatrix() {
    transformation->UpdateModelMatrix();
}

void Block::UpdateModelMatrix(const glm::mat4 &_parentTransformationMatrix) {
    transformation->UpdateModelMatrix(_parentTransformationMatrix);
}