//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_CUBE_H
#define UNTITLED7_CUBE_H

#include <memory>
#include <vector>
#include <string>

#include <glew.h>
#include <glm/matrix.hpp>

#include "Texture.h"

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

        // Vertex Data
        std::unique_ptr<Vertex> origin {};
        std::vector<GLuint> indexArray {};
        std::unique_ptr<std::vector<Vertex>> vertexArray {};
        std::unique_ptr<std::vector<VertexOffsets>> vertexOffsetArray {};
        glm::vec3 dimensions {1.0f, 1.0f, 1.0f};

        // Display Data
        Texture* texture {};

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
        void SetTexture(Texture* _texture, glm::vec2 _sheetPosition);

        // Positioning
        void SetPositionOrigin(glm::vec3 _origin);
        void SetPositionCentre(glm::vec3 _centre);
        void SetTextureOrigin(glm::vec2 _origin);
        void SetDimensions(glm::vec3 _dimensions);

        // Getters
        [[nodiscard]] glm::vec3 GetDimensions() { return dimensions; }
};

#endif //UNTITLED7_CUBE_H
