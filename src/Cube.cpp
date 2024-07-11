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

    // [POSITION], [TEXCOORD], both values are relative to the set origin points
    std::vector<VertexOffsets> vertexVectors = {
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

    // Populate vertexOffsetArray with data
    vertexOffsetArray = std::make_unique<std::vector<VertexOffsets>>(vertexVectors);

    // Set default origin position
    origin = std::make_unique<Vertex>(Vertex{glm::vec3(0.0f, 0.0f, 0.0f),
                                             glm::vec2(0.0f, 0.0f),
                                             glm::vec3(1.0f, 1.0f, 1.0f)});

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

void Cube::UpdateVertexPositions() const {
    // Update vertex position data
    for (int v = 0; v < vertexArray->size(); v++) {
        glm::vec3 offset = vertexOffsetArray->at(v).positionOffset;
        vertexArray->at(v).position.x = origin->position.x + (offset.x * dimensions[0]);
        vertexArray->at(v).position.y = origin->position.y + (offset.y * dimensions[1]);
        vertexArray->at(v).position.z = origin->position.z + (offset.z * dimensions[2]);
    }

    // Bind data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::UpdateColorBuffer() const {
    // Set vertex with new colours
    // ...

}

void Cube::UpdateVertexTextureCoords() const {
    if (texture == nullptr) return;

    // Update vector texture coordinates with new correct value
    for (int v = 0; v < vertexArray->size(); v++) {
        std::pair<float, float> texturePos = texture->GetTextureSheetTile(origin->texture + vertexOffsetArray->at(v).textureOffset);

        vertexArray->at(v).texture = {texturePos.first, texturePos.second};
    }

    // Bind data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray->size() * sizeof(struct Vertex)), vertexArray->data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





void Cube::Display() const {
    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Enable Attributes
    glEnable(GL_DEPTH_TEST);

    // Activate texture
    if (texture != nullptr) texture->EnableTexture();

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // unbind
    if (texture != nullptr) texture->DisableTexture();
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

void Cube::SetTexture(Texture* _texture, glm::vec2 _sheetPosition) {
    if (_texture == nullptr) {
        printf("Texture cannot be assigned to null\n");
        return;
    }

    // WARNING ENSURE CHECK FOR NULL TEXTURE BEFORE USING IN DISPLAY
    texture = _texture;

    // Set offset of texture position in texturesheet. non-sheets will use (0.0f, 0.0f).
    origin->texture = _sheetPosition;
    UpdateVertexTextureCoords();
}


void Cube::SetPositionOrigin(glm::vec3 _origin) {
    origin->position =_origin;

    // Update the vertex positions
    UpdateVertexPositions();
}

void Cube::SetPositionCentre(glm::vec3 _centre) {
    origin->position = _centre - (dimensions/2.0f);
    origin->position.y += dimensions.y;

    // Update the vertex positions
    UpdateVertexPositions();
}

void Cube::SetTextureOrigin(glm::vec2 _origin) {
    origin->texture = _origin;

    // Update the texture positions
    UpdateVertexTextureCoords();
}

void Cube::SetDimensions(glm::vec3 _dimensions) {
    dimensions = _dimensions;

    // Update vertex positions
    UpdateVertexPositions();
}