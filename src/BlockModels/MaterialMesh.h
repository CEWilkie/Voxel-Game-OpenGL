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
        bool unboundChanges = false;
        bool bound = false;

    public:
        explicit MaterialMesh(Block* _block);
        ~MaterialMesh();

        void AddVerticies(const std::vector<Vertex>& _verticies, const glm::vec3& _position);
        void RemoveVerticies(std::vector<Vertex>& _verticies, const glm::vec3& _position);
        void ResetVerticies();

        void BindMesh();
        void UpdateMesh();
        void DrawMesh(const Transformation& _transformation) const;



        [[nodiscard]] Block* GetBlock() const { return block; }
        [[nodiscard]] bool UnboundChanges() const {return unboundChanges; }
        [[nodiscard]] bool Bound() const { return bound; }
};


#endif //UNTITLED7_MATERIALMESH_H
