//
// Created by cew05 on 26/07/2024.
//

#ifndef UNTITLED7_MATERIALMESH_H
#define UNTITLED7_MATERIALMESH_H

#include <glew.h>
#include "Block.h"

class MaterialMesh {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int indexBufferObject {};

        std::vector<Vertex> vertexArray {};
        std::vector<GLuint> indexArray {};
        int nFaces = 0;

        TEXTURESHEET texturesheet = TEXTURESHEET::NATURAL;

    public:
        MaterialMesh();
        ~MaterialMesh();

        void AddBlockFaceVertex(BLOCKFACE _faceID, glm::vec3 _position);
        void RemoveBlockFaceVertex(BLOCKFACE _faceID, glm::vec3 _position);

        void BindMesh();
        void DrawMesh(const Transformation& _transformation);
};


#endif //UNTITLED7_MATERIALMESH_H
