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

#include "../BlockModels/ModelStructs.h"

class Camera {
    private:
        // Global perspective matrix
        glm::mat4 perspective {};

        // Camera vector info
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

        // View Frustrum vars
        float fovAngleY = 45.0f;
        float minDistance = 0.1f;
        float maxDistance = 16*16.0f;

    public:
        Camera();

        // Camera Movement
        void Move(Uint64 _deltaFrames);
        void MouseLook(SDL_bool _mouseGrabbed);
        void UpdateLookatUniform() const;

        // Frustrum Culling with view clip planes
        void UpdateViewFrustrum();

        // Camera Setters
        void MoveTo(const glm::vec3& _position);
        void SetDirection(const glm::vec3& _direction);
        void SetAngle(double _angleVert, double _angleHoriz);

        // Getters
        [[nodiscard]] glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position, position+direction, normalUp);
        }
        [[nodiscard]] glm::vec3 GetDirection() const { return direction; }
        [[nodiscard]] std::pair<double, double> GetAngle() { return {angleVert, angleHoriz}; }
        [[nodiscard]] std::pair<float, float> GetMinMaxDistance() const { return {minDistance, maxDistance}; }
        [[nodiscard]] glm::vec3 GetPosition() const { return position; }
        [[nodiscard]] Frustrum GetCameraFrustrum() const { return vf; }
};

#endif //UNTITLED7_CAMERA_H
