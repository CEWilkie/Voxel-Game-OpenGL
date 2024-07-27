//
// Created by cew05 on 26/07/2024.
//

#include "ChunkMesh.h"

#include "../Window.h"

ChunkMesh::ChunkMesh() {
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &indexBufferObject);
}

ChunkMesh::~ChunkMesh() {
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &indexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void ChunkMesh::AddBlockFaceVertex(BLOCKFACE _faceID, glm::vec3 _position) {
//    std::vector<Vertex> faceVerticies = BlockVAOs::GetFaceVerticies(_faceID);
//
//    // Iter over all verticies to find unique and update data
//    for (auto& vertex : faceVerticies) {
//
//        // Update vertex data
//        vertex.textureCoord = textureManager->GetTextureData(texturesheet)->GetTextureSheetTile(vertex.textureCoord);
//        vertex.position += _position;
//
//        // Add to vertex array
//        vertexArray.push_back(vertex);
//    }
//
//    nFaces += 1;
//
//    // Add indexes
//    for (int i = 0; i < 4; i++) indexArray.push_back(i+vertexArray.size());
}

void ChunkMesh::RemoveBlockFaceVertex(BLOCKFACE _faceID, glm::vec3 _position) {
    // ...
}

void ChunkMesh::BindMesh() {
    glBindVertexArray(vertexArrayObject);

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

    GLint tex0Location = glGetUniformLocation(window.GetShader(), "tex0");
    glUniform1i(tex0Location, 0);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkMesh::DrawMesh(const Transformation& _transformation) {
    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Update uniform
    GLint modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uModelMatrix");
    if (modelMatrixLocation < 0) printf("mesh location not found [uModelMatrix]\n");
    if (modelMatrixLocation >= 0) glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &_transformation.GetModelMatrix()[0][0]);

    modelMatrixLocation = glGetUniformLocation(window.GetShader(), "uVertexTextureCoordOffset");
    if (modelMatrixLocation < 0) printf("sun location not found [uVertexTextureCoordOffset]\n");
    glm::vec2 o(1.0f, 1.0f);
    if (modelMatrixLocation >= 0) glUniform2fv(modelMatrixLocation, 1, &o[0]);

    textureManager->EnableTextureSheet(texturesheet);
    glDrawElements(GL_QUADS, 4*nFaces, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}