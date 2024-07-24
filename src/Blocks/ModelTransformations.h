//
// Created by cew05 on 12/07/2024.
//

#ifndef UNTITLED7_MODELTRANSFORMATIONS_H
#define UNTITLED7_MODELTRANSFORMATIONS_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transformation {
    private:
        // Pure transformations info
        glm::vec3 position {0.0f, 0.0f, 0.0f};
        glm::vec3 rotation {0.0f, 0.0f, 0.0f};  // Degrees
        glm::vec3 scale {1.0f, 1.0f, 1.0f};

        // Transformation matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        // Checker for when modelMatrix is updated
        bool modelUpdated = false;

    public:
        // Getting the transformation matrix of the object
        [[nodiscard]] glm::mat4 GetLocalTransformationMatrix() {
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 rotationFull = rotateX * rotateY * rotateZ;

            return glm::translate(glm::mat4(1.0f), position) * rotationFull * glm::scale(glm::mat4(1.0f), scale);
        }

        // No parent model
        void UpdateModelMatrix() {
            modelMatrix = GetLocalTransformationMatrix();
            modelUpdated = true;
        }

        // Account for transformation of parent model
        void UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix) {
            modelMatrix = _parentTransformationMatrix * GetLocalTransformationMatrix();
            modelUpdated = true;
        }

        // Fetch model matrix (may not be updated)
        [[nodiscard]] glm::mat4 GetModelMatrix() const { return modelMatrix; }

        void SetPosition(const glm::vec3& _position) {
            position = _position;
            modelUpdated = false;
        }
        void SetRotation(const glm::vec3& _rotation) {
            rotation = _rotation;
            modelUpdated = false;
        }
        void SetScale(const glm::vec3& _scale) {
            scale = _scale;
            modelUpdated = false;
        }

        // Normals
        [[nodiscard]] glm::vec3 GetUp() const { return modelMatrix[1]; }
        [[nodiscard]] glm::vec3 GetRight() const { return modelMatrix[0]; }
        [[nodiscard]] glm::vec3 GetForward() const { return modelMatrix[2]; }

        // Fetching Transformation Info
        [[nodiscard]] glm::vec3 GetGlobalPosition() const { return modelMatrix[3]; }
        [[nodiscard]] glm::vec3 GetLocalPosition() const { return position; }
        [[nodiscard]] glm::vec3 GetLocalRotation() const { return rotation; }
        [[nodiscard]] glm::vec3 GetGlobalScale() const {
            return {glm::length(GetRight()), glm::length(GetUp()),glm::length(-GetForward())}; }
        [[nodiscard]] glm::vec3 GetLocalScale() const { return scale; }

        // Has transformation info been updated, and model not yet updated
        [[nodiscard]] bool ModelUpdated() { return modelUpdated; }
};


#endif //UNTITLED7_MODELTRANSFORMATIONS_H
