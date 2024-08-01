//
// Created by cew05 on 01/08/2024.
//

#ifndef UNTITLED7_PLAYER_H
#define UNTITLED7_PLAYER_H

#include <glm/matrix.hpp>
#include "Camera.h"
#include "../World/World.h"

class World;

class Player {
    private:
        // Positioning info
        glm::vec3 position {};
        float maxHorizSpeed {};
        float maxVertSpeed {};
        float horizSpeed {};
        float vertSpeed {};
        glm::vec3 normalUp {};
        glm::vec3 normalRight {};

        // Camera management
        glm::vec3 facingDirection {};
        float cameraSensitivity {};
        bool cameraMoved = true;

        std::unique_ptr<Camera> firstPerson = std::make_unique<Camera>();
        std::unique_ptr<Camera> thirdPerson = std::make_unique<Camera>();
        Camera* usingCamera {};

    public:
        Player(glm::vec3 _position, glm::vec3 _facingDirection);

        // Movement
        void Move(Uint64 _deltaFrames);
        void MouseLook(SDL_bool _mouseGrabbed);

        // Getters
        [[nodiscard]] Camera* GetUsingCamera() const { return usingCamera; }
        [[nodiscard]] glm::vec3 GetPosition() const { return position; }
        [[nodiscard]] bool CameraMoved() const { return cameraMoved; }
};


#endif //UNTITLED7_PLAYER_H
