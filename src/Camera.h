//
// Created by cew05 on 08/07/2024.
//

#ifndef UNTITLED7_CAMERA_H
#define UNTITLED7_CAMERA_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

class Camera {
    private:
        // Global perspective
        glm::mat4 perspective {};

        // Camera info
        glm::vec3 position {};
        glm::vec3 direction {};
        glm::vec3 normalUp {};

        // Camera Direction Info
        double angleVert = 0;
        double angleHoriz = 0;
        double sensitivity = 0.1;



    public:
        Camera();

        void MoveTo(const glm::vec3& _position);
        void Move(Uint64 _deltaFrames);
        void MouseLook(SDL_bool _mouseGrabbed);
        void UpdateUniform() const;

        // Getters
        [[nodiscard]] glm::mat4 GetViewMatrix() const {
            return glm::lookAt(position, position+direction, normalUp);
        }

        [[nodiscard]] glm::vec3 GetFacing() const {
            return direction;
        }
};

#endif //UNTITLED7_CAMERA_H
