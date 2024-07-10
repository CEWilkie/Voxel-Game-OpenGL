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
    glm::vec3 color {};
    glm::vec3 textureCoord {};
};

class Cube {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int colorBufferObject {};
        unsigned int indexBufferObject {};
        unsigned int textureBufferObject {};

        // Vertex Data
        std::vector<float> vertexArray {};
        std::vector<GLuint> indexArray {};

        // Display Data
        std::vector<float> vertexColorArray {};
        std::vector<float> vertexTextureArray {};

    public:
        Cube();
        ~Cube();

        void BindCube();

        // Display
        void Display() const;
        void CreateTextures() const;

        // Positioning
        void SetPosition(const std::vector<float>& _originVertex);
};

#endif //UNTITLED7_CUBE_H
