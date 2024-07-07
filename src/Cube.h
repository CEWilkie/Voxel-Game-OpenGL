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

        // Normal Vector Data
        glm::vec3 xNorm {};
        glm::vec3 yNorm {};
        glm::vec3 zNorm {};

        // Display Data
        std::vector<float> vertexColorArray {};

    public:
        Cube();
        ~Cube();

        void BindCube();

        // Display
        void Display() const;

        // Positioning
        void Rotate(const int _dimension, float _theta);
        void Move(const std::vector<float>& _dist);
};

#endif //UNTITLED7_CUBE_H
