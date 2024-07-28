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

        Block* block;

    public:
        explicit MaterialMesh(Block* _block);
        ~MaterialMesh();

        void AddVerticies(std::vector<Vertex> _verticies, glm::vec3 _position);
        void RemoveBlockFaceVertex(BLOCKFACE _faceID, glm::vec3 _position);

        void BindMesh();
        void DrawMesh(const Transformation& _transformation);



        [[nodiscard]] Block* GetBlock() const { return block; }
};


#endif //UNTITLED7_MATERIALMESH_H
