//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_CUBE_H
#define UNTITLED7_CUBE_H

#include <memory>
#include <vector>

#include <glm/matrix.hpp>

#include "Quad.h"

struct Vertex {
    glm::vec3 position {};
    glm::vec2 texture {};
    glm::vec3 color {};
};

struct VertexOffsets {
    glm::vec3 positionOffset {};
    glm::vec2 textureOffset {};
};

class Cube {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int indexBufferObject {};
        unsigned int textureBufferObject {};

        // Vertex Data
        Vertex origin {};
        std::vector<GLuint> indexArray {};
        std::unique_ptr<std::vector<Vertex>> vertexArray {};
        std::unique_ptr<std::vector<VertexOffsets>> vertexOffsetArray {};

        // Display Data
        std::pair<int, int> textureSheetSize {256, 256};
        std::pair<int, int> textureSheetGrid {4, 4};

        // Bind Data to openGL
        void BindCube() const;

        void UpdateVertexPositions() const;
        void UpdateColorBuffer() const;
        void UpdateVertexTextureCoords() const;

    public:
        Cube();
        ~Cube();

        // Display
        void Display() const;
        void CreateTextures() const;

        // Set Relative Position Origins
        void SetPositionOrigin(glm::vec3 _origin);
        void SetTextureOrigin(glm::vec2 _origin);
};

#endif //UNTITLED7_CUBE_H
