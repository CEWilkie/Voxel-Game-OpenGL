//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_CUBE_H
#define UNTITLED7_CUBE_H

#include <memory>
#include <vector>

#include <glm/matrix.hpp>

#include "Quad.h"

class Cube {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int colorBufferObject {};
        unsigned int indexBufferObject {};

        // Vertex Data
        std::vector<float> vertexArray {};
        std::vector<GLuint> indexArray {};
        glm::vec3 centre {};

        // Display Data
        std::vector<float> vertexColorArray {};

    public:
        Cube();
        ~Cube();

        void BindCube();

        // Display
        void Display() const;

        // Positioning
        void Rotate(float _theta);
};

#endif //UNTITLED7_CUBE_H
