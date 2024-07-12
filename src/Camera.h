//
// Created by cew05 on 08/07/2024.
//

#ifndef UNTITLED7_CAMERA_H
#define UNTITLED7_CAMERA_H

#include <glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <memory>

#include "ModelStructs.h"

class Camera {
    private:
        // Global perspective
        glm::mat4 perspective {};
        float fovAngleY = 45.0f;
        float minDistance = 1.0f;
        float maxDistance = 16*16.0f;

        // Camera info
        glm::vec3 position {};
        glm::vec3 direction {};
        glm::vec3 normalUp {};
        glm::vec3 normalRight {};

        // Camera Clipping Planes
        Frustrum vf {};

        // Camera Direction Info
        double angleVert = 0;
        double angleHoriz = 0;
        double sensitivity = 0.1;

        // XYZ Vertex Direction Display
        unsigned int vertexArrayObject {};
        unsigned int vertexBufferObject {};

        // Direction Vertex Info
        std::vector<float> vertexArray {};

        void BindDirectionVertexes() const;

        float maxy = 0.5f;

    public:
        Camera();

        // Direction Vertexes
        void DisplayDirectionVertexes() const;

        // Camera Movement
        void MoveTo(const glm::vec3& _position);
        void Move(Uint64 _deltaFrames);
        void MouseLook(SDL_bool _mouseGrabbed);
        void UpdateUniform() const;

        // Frustrum Culling with view clip planes
        void UpdateViewFrustrum();
        [[nodiscard]] bool ObjectInView(const BoundingVolume &_volume) const;

        // Getters
        [[nodiscard]] glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position, position+direction, normalUp);
        }
        [[nodiscard]] glm::vec3 GetFacing() const { return direction; }
        [[nodiscard]] std::pair<float, float> GetMinMaxDistance() const { return {minDistance, maxDistance}; }
        [[nodiscard]] glm::vec3 GetPosition() const { return position; }
        [[nodiscard]] Frustrum GetCameraFrustrum() const { return vf; }
};

#endif //UNTITLED7_CAMERA_H
