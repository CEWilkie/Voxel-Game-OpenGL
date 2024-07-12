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
#include "ModelStructs.h"
#include "Camera.h"

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
        std::unique_ptr<std::vector<Vertex>> vertexOffsetArray {};
        glm::vec3 dimensions {};

        // Display and Culling info
        Texture* texture {};
        SphereBounds* sphereBounds {};
        BoxBounds* boxBounds {};
        bool canDisplay = true;

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
        bool CheckCulling(const Camera& _camera);
        void SetTexture(Texture* _texture, glm::vec2 _sheetPosition);

        // Positioning
        void SetPositionOrigin(glm::vec3 _origin);
        void SetPositionCentre(glm::vec3 _centre);
        void SetTextureOrigin(glm::vec2 _origin);
        void SetDimensions(glm::vec3 _dimensions);

        // Getters
        [[nodiscard]] glm::vec3 GetDimensions() { return dimensions; }
        [[nodiscard]] glm::vec3 GetCentre() { return origin->position + dimensions/2.0f; }
};

#endif //UNTITLED7_CUBE_H
