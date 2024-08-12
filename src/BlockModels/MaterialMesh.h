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
        int nFaces = 0;

        Block* block;
        bool unmeshedChanges = true;

    public:
        explicit MaterialMesh(Block* _block);
        ~MaterialMesh();

        void AddVerticies(std::vector<Vertex> _verticies, glm::vec3 _position);
        void RemoveVerticies(std::vector<Vertex> _verticies, glm::vec3 _position);
        void ResetVerticies();

        void BindMesh();
        void UpdateMesh();
        void DrawMesh(const Transformation& _transformation) const;



        [[nodiscard]] Block* GetBlock() const { return block; }
        [[nodiscard]] bool OldMesh() const {return unmeshedChanges; }
};


#endif //UNTITLED7_MATERIALMESH_H
