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
#include "ModelTransformations.h"
#include "Camera.h"

class Cube {
    private:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int indexBufferObject {};

        // Vertex Data
        std::vector<GLuint> indexArray {};
        std::unique_ptr<std::vector<Vertex>> vertexArray {};
        std::unique_ptr<Transformation> transformation {};
        GLint modelMatrixLocation = -1;

        // Display and Textures
        Texture* texture {};
        glm::vec2 textureOrigin {};

        // Culling Boundaries
        SphereBounds* sphereBounds {};
        BoxBounds* boxBounds {};
        bool canDisplay = true;

        // Bind Data to openGL
        void BindCube() const;

        void UpdateTextureData();

    public:
        Cube();
        ~Cube();

        // Display
        void Display() const;
        void SetTexture(Texture* _texture, glm::vec2 _origin);
        void SetTextureOrigin(glm::vec2 _origin);

        // Transformations
        void SetPositionOrigin(glm::vec3 _originPosition);
        void SetPositionCentre(glm::vec3 _centre);
        void SetScale(glm::vec3 _scale);
        void UpdateModelMatrix();

        // Object culling
        bool CheckCulling(const Camera& _camera);

        // Getters
        [[nodiscard]] glm::vec3 GetDimensions() { return transformation->GetLocalScale(); }
        [[nodiscard]] glm::vec3 GetGlobalCentre() { return transformation->GetGlobalPosition() + GetDimensions() / 2.0f; }
        [[nodiscard]] glm::vec3 GetLocalCentre() { return transformation->GetLocalPosition() + GetDimensions() / 2.0f; }
};

#endif //UNTITLED7_CUBE_H
