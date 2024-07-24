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

#include "../Textures/TextureData.h"
#include "ModelStructs.h"
#include "ModelTransformations.h"
#include "../Player/Camera.h"
#include "../Textures/TextureManager.h"

class Cube {
    protected:
        // Buffer objects
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};
        unsigned int indexBufferObject {};

        // Vertex Data
        std::vector<GLuint> indexArray {};
        std::unique_ptr<std::vector<Vertex>> vertexArray {};
        std::unique_ptr<Transformation> transformation {};
        GLint modelMatrixLocation = -1;

        // Culling Information
        std::unique_ptr<BoxBounds> boxBounds {};
        bool isCulled = false;

        // Texture information
        TEXTURESHEET textureSheetID = TEXTURESHEET::TEST16;
        glm::vec2 textureOrigin {1, 1};

        // Bind Data to openGL
        void BindCube() const;
        void UpdateTextureData();

    public:
        Cube();
        ~Cube();

        // Display
        void Display() const;

        // Object culling
        bool CheckCulling(const Camera& _camera);

        // Textures
        void SetTexture(TEXTURESHEET _textureID, glm::vec2 _origin);
        void SetTextureOrigin(glm::vec2 _origin);

        // Transformations
        void SetPositionOrigin(glm::vec3 _originPosition);
        void SetPositionCentre(glm::vec3 _centre);
        void SetScale(glm::vec3 _scale);
        void SetRotation(glm::vec3 _rotation);
        void UpdateModelMatrix();
        void UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix);

        // Getters
        [[nodiscard]] glm::vec3 GetDimensions() { return transformation->GetLocalScale(); }
        [[nodiscard]] glm::vec3 GetGlobalCentre() { return transformation->GetGlobalPosition() + GetDimensions() / 2.0f; }
        [[nodiscard]] glm::vec3 GetLocalCentre() { return transformation->GetLocalPosition() + GetDimensions() / 2.0f; }
        [[nodiscard]] std::vector<Vertex> GetVertexArray() { return *vertexArray; }
        [[nodiscard]] std::pair<glm::vec3, glm::vec3> GetMinMaxGlobalBounds() const {
            return boxBounds->GetMinMaxGlobalVertex(*transformation); }
};

#endif //UNTITLED7_CUBE_H
