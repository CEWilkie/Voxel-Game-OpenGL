//
// Created by cew05 on 12/07/2024.
//

#ifndef UNTITLED7_MODELTRANSFORMATIONS_H
#define UNTITLED7_MODELTRANSFORMATIONS_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
 * Transformations permits interactions and use with an objects positioning, rotation and scale. The transformations
 * model matrix should be passed to the uniform uModelMatrix when displaying an object. Transformations may also be
 * relative, and take into account a given parent transformation when updating the model matrix
 */

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
        [[nodiscard]] bool ModelUpdated() const { return modelUpdated; }
};




// DIRECTIONS FOR CHECKING ADJACENT CHUNKS / BLOCKS / BIOMES
inline glm::vec3 dirTop{0, 1, 0};
inline glm::vec3 dirBottom{0, -1, 0};
inline glm::vec3 dirFront{-1, 0, 0};
inline glm::vec3 dirFrontLeft{-1, 0, -1};
inline glm::vec3 dirLeft{0, 0, -1};
inline glm::vec3 dirBackLeft{1, 0, -1};
inline glm::vec3 dirBack{1, 0, 0};
inline glm::vec3 dirBackRight{1, 0, 1};
inline glm::vec3 dirRight{0, 0, 1};
inline glm::vec3 dirFrontRight{-1, 0, 1};

// list of all directions
const int numDirections = 10;
inline std::array<glm::vec3, numDirections> allDirections { dirTop, dirBottom, dirFront, dirFrontLeft, dirLeft, dirBackLeft,
                                                            dirBack, dirBackRight, dirRight, dirFrontRight};

// enums for directions
enum DIRECTION : int {
    UP, DOWN, SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, EAST, SOUTHEAST,
};

// get direction vector from enum
inline glm::vec3 GetDirection(const DIRECTION& _direction) {
    switch (_direction) {
        case UP:
            return dirTop;
        case DOWN:
            return dirBottom;

        case SOUTH:
            return dirFront;
        case NORTH:
            return dirBack;
        case EAST:
            return dirRight;
        case WEST:
            return dirLeft;

        case SOUTHEAST:
            return dirFrontRight;
        case SOUTHWEST:
            return dirFrontLeft;
        case NORTHEAST:
            return dirBackRight;
        case NORTHWEST:
            return dirBackLeft;
    }
}


#endif //UNTITLED7_MODELTRANSFORMATIONS_H
