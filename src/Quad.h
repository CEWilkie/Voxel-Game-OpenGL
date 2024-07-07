//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_QUAD_H
#define UNTITLED7_QUAD_H

#include <vector>
#include <random>
#include <glew.h>

#include "CoreGlobals.h"
#include "Window.h"

class Quad {
    private:
        // GL bindings
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int colorBufferObject {};
        unsigned int indexBufferObject {};

        // Vertex Data
        bool dataBound = false;
        GLint numAttribs{};
        std::vector<float> vertexArray {};
        std::vector<GLuint> indexBufferData {};

        // Display
        std::vector<float> vertexColorArray {};

        // Movement
        std::pair<float, float> vel {0.005f, 0.008f};

    public:
        // Constructors
        Quad();
        ~Quad();
        void ConstructQuad();

        void SetVertexArray(const std::vector<float>& _vertexArray);

        // Movement
        void Move();

        // Display
        void Display() const;
};


#endif //UNTITLED7_QUAD_H
