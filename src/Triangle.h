//
// Created by cew05 on 16/04/2024.
//

#ifndef UNTITLED7_CUBE_H
#define UNTITLED7_CUBE_H

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>

#include "CoreGlobals.h"

class Triangle {
    private:
        // GL bindings
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int textureObject {};

        // Vertex Data
        GLint numAttribs{};
        std::vector<float> vertexArray {};

        // Display


    public:
        Triangle();
        ~Triangle();
        void ConstructTriangle(GLint _numAttribs, const std::vector<float>& _vertexArray);
        void Display() const;
};


#endif //UNTITLED7_CUBE_H
