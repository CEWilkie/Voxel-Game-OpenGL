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


struct Kinematics {
    glm::vec3 initPosition {};          //          // initial position movement starts from (NOT DISPLACEMENT)
    glm::vec3 initVelocity {};          // u        // velocity at start of a movement
    glm::vec3 finalVelocity {};         // v        // velocity at end of a movement
    glm::vec3 acceleration {};          // a        // acceleration of velocity
    float seconds;                      // t        // time measurement for equations

    // Set initial values for an entity's movement, assumes s = 0, t = 0;
    void StartMovement(glm::vec3 _p, glm::vec3 _u, glm::vec3 _a) {
        initPosition = _p;
        initVelocity = _u;
        finalVelocity = {0,0,0};
        acceleration = _a;
        seconds = 0;
    }

    // Update the values being used in the movement without restarting the movement time
    void UpdateMovementValues(glm::vec3 _p, glm::vec3 _u, glm::vec3 _a) {
        initPosition = _p;
        initVelocity = _u;
        acceleration = _a;
    }

    // returns the new position modeled from init position of the current movement's u, a, and sum time t
    glm::vec3 GetNewPosition(float _seconds) {
        seconds += _seconds; // sum of seconds in jump
        finalVelocity = initVelocity + acceleration * seconds; // v = u + at
        return (initVelocity * seconds) + (0.5f * acceleration * std::pow(seconds, 2.0f)) + initPosition; // new p = ut + 0.5a(t^2) + p
    }

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
