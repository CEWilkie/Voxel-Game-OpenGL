//
// Created by cew05 on 26/07/2024.
//

#ifndef UNTITLED7_MATERIALMESH_H
#define UNTITLED7_MATERIALMESH_H

#include <glew.h>
#include "Block.h"

/*
 * Used to merge together multiple instances of a singular block type into a single mesh of verticies. Permits far
 * greater optimisation of drawing. Verticies for the object should be provided by determining the visible faces at a
 * given position, and passing that position and the vertex data of the visible faces to AddVerticies.
 */

class MaterialMesh {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int indexBufferObject {};

        std::vector<Vertex> vertexArray {};
        int bufferVerticiesSize = 0;
        int boundFaces = 0;

        Block* block;
        bool oldMesh = true;
        bool readyToBind = false;

    public:
        explicit MaterialMesh(Block* _block);
        ~MaterialMesh();

        // Mesh verticies setup and binding
        void AddVerticies(const std::vector<Vertex>& _verticies, const glm::vec3& _position);
        void ResetVerticies();
        void BindMesh();
        void UpdateMesh();

        // Mark meshes for recreation
        void MarkOld() { oldMesh = true; }
        void MarkReadyToBind() {readyToBind = true; }
        [[nodiscard]] bool IsOld() const { return oldMesh; }
        [[nodiscard]] bool ReadyToBind() const { return readyToBind; }

        // Mesh Display
        void DrawMesh(const Transformation& _transformation) const;

        // Getters
        [[nodiscard]] Block* GetBlock() const { return block; }
};


#endif //UNTITLED7_MATERIALMESH_H
