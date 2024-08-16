//
// Created by cew05 on 26/07/2024.
//

#include "MaterialMesh.h"

#include "../Window.h"

MaterialMesh::MaterialMesh(Block* _block) {
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &indexBufferObject);

    block = _block;
}

MaterialMesh::~MaterialMesh() {
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &indexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void MaterialMesh::AddVerticies(const std::vector<Vertex>& _verticies, const glm::vec3& _position) {
    for (const Vertex& vertex : _verticies) {
        vertexArray.push_back({vertex.position + _position, vertex.textureCoord});
    }
}

void MaterialMesh::ResetVerticies() {
    vertexArray.clear();
    nFaces = 0;
}

void MaterialMesh::BindMesh() {
    glBindVertexArray(vertexArrayObject);

    // populate indexArray
    nFaces = (int)vertexArray.size() / 4;
    std::vector<GLuint> indexArray {};
    for (int f = 0; f < nFaces; f++) {
        indexArray.push_back(f*4 + 1);
        indexArray.push_back(f*4 + 3);
        indexArray.push_back(f*4 + 0);
        indexArray.push_back(f*4 + 0);
        indexArray.push_back(f*4 + 3);
        indexArray.push_back(f*4 + 2);
    }

//    printf("v %zu i %zu\n", vertexArray.size(), indexArray.size());

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(Vertex)), vertexArray.data(), GL_STATIC_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, position));

    // Vertex TextureData attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(Vertex, textureCoord));

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data(), GL_STATIC_DRAW);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MaterialMesh::UpdateMesh() {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray.size() * sizeof(Vertex)), vertexArray.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // populate indexArray
    nFaces = (int)vertexArray.size() / 4;
    std::vector<GLuint> indexArray {};
    for (int f = 0; f < nFaces; f++) {
        indexArray.push_back(f*4 + 1);
        indexArray.push_back(f*4 + 3);
        indexArray.push_back(f*4 + 0);
        indexArray.push_back(f*4 + 0);
        indexArray.push_back(f*4 + 3);
        indexArray.push_back(f*4 + 2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MaterialMesh::DrawMesh(const Transformation& _transformation) const {
    if (vertexArray.empty()) return; // nothing to draw

    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Update uniform
    GLint modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uModelMatrix");
    if (modelMatrixLocation < 0) printf("mesh location not found [uModelMatrix]\n");
    if (modelMatrixLocation >= 0) glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &_transformation.GetModelMatrix()[0][0]);

    // Set texture information
    textureManager->EnableTextureSheet(block->GetTextureSheet());
    GLint vtcOffsetLocation = glGetUniformLocation(window.GetShader(), "uVertexTextureCoordOffset");
    if (vtcOffsetLocation < 0) printf("sun location not found [uVertexTextureCoordOffset]\n");
    if (vtcOffsetLocation >= 0) glUniform2fv(vtcOffsetLocation, 1, &block->GetTextureOrigin()[0]);

    // Draw block mesh
    glDrawElements(GL_TRIANGLES, 6*nFaces, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}