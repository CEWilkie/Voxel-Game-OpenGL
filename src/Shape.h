//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_SHAPE_H
#define UNTITLED7_SHAPE_H

#include <vector>

#include <SDL.h>
#include <glew.h>

class Shape {
    protected:
        // Position data
        SDL_Rect shapeRect {};

        // GL bindings
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int colorBufferObject {};

        // Vertex Data
        GLint vertexPositionAttribs = 3;
        std::vector<float> vertexArray {};

        // Display
        GLint vertexColorAttribs = 3;
        std::vector<float> vertexColorArray {};

        // Movement
        std::pair<float, float> vel {0.005f, 0.008f};

    public:
        // Constructors
        Shape();
        ~Shape();
        void SetVertexArray(const std::vector<float>& _vertexArray);


        // Display
        void Display() const;
};


#endif //UNTITLED7_SHAPE_H
