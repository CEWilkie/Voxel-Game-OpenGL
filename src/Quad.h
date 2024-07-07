//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_QUAD_H
#define UNTITLED7_QUAD_H

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>

#include "CoreGlobals.h"
#include "Window.h"

class Quad {
    private:
        // GL bindings
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};

        // Vertex Data
        GLint numAttribs{};
        std::vector<float> vertexArray {};

        // Display
        unsigned int textureObject {};
        unsigned int colorBufferObject {};
        std::vector<float> vertexColorArray {};

        // Movement
        std::pair<float, float> vel {0.005f, 0.008f};

    public:
        // Constructors
        Quad();
        ~Quad();
        void ConstructQuad();

        // Movement
        void Move();

        // Display
        void Display() const;
};


#endif //UNTITLED7_QUAD_H
