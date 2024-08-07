//
// Created by cew05 on 01/08/2024.
//

#ifndef UNTITLED7_PLAYER_H
#define UNTITLED7_PLAYER_H

#include <cmath>
#include <glm/matrix.hpp>
#include "Camera.h"
#include "../World/World.h"

class World;

enum class MOVEMENTMODE {
        WALKING, FLYING, // ...
};

class Player {
    private:
        // Positioning info
        glm::vec3 position {};
        glm::vec3 lastPosition {};
        glm::vec3 lastStaticPosition {};
        float topLevel {};
        float timeSinceOnGround {};
        float timeSinceStill {};
        float radius = 0.4f;
        float maxX {}, minX {};
        float maxZ {}, minZ {};
        float maxY {}, minY {};

        // Movement Info
        MOVEMENTMODE movementMode = MOVEMENTMODE::WALKING;
        glm::vec3 vectorSpeed {};
        glm::vec3 vectorAcceleration {};
        glm::vec3 moveDirection {0, 0, 0};


        Kinematics kinematics {};

        // Player normals
        glm::vec3 normalUp {};
        glm::vec3 normalRight {};
        glm::vec3 normalFront {};

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
        void HandleMovement(Uint64 _deltaTicks);
        void EnforcePositionBoundaries(float _seconds);
        void UpdateMaxPositions();
        void FlyingMovement(float _seconds);
        void WalkingMovement(float _seconds);
        void MouseLook(SDL_bool _mouseGrabbed);

        // Getters
        [[nodiscard]] Camera* GetUsingCamera() const { return usingCamera; }
        [[nodiscard]] glm::vec3 GetPosition() const { return position; }
        [[nodiscard]] bool CameraMoved() const { return cameraMoved; }
};


#endif //UNTITLED7_PLAYER_H
