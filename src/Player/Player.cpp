//
// Created by cew05 on 01/08/2024.
//

#include "Player.h"
#include "../Window.h"

Player::Player(glm::vec3 _position, glm::vec3 _facingDirection) {
    position = _position;
    lastStaticPosition = position;
    facingDirection = _facingDirection;
    normalUp = glm::vec3(0.0f, 1.0f, 0.0f);
    normalRight = glm::vec3(1.0f, 0.0f, 0.0f);

    // Create Cameras
    firstPerson->SetDirection(facingDirection);
    firstPerson->MoveTo({position.x, position.y + 1, position.z});
    thirdPerson->SetDirection(facingDirection);
    thirdPerson->MoveTo({position.x, position.y + 1, position.z});

    usingCamera = firstPerson.get();
    usingCamera->UpdateViewFrustrum();
    usingCamera->UpdateLookatUniform();

    cameraSensitivity = 0.1f;

    UpdateMaxPositions();
}

void Player::HandleMovement(Uint64 _deltaTicks) {
    // Seconds that have passed since last movement update
    float seconds = float(_deltaTicks) / 1000.0f;

    // Update player position using correct movement method
    switch (movementMode) {
        case MOVEMENTMODE::FLYING:
            FlyingMovement(seconds);
            break;

        default: // WALKING
            WalkingMovement(seconds);
    }

    // Ensure that the new position remains within the boundaries (not travel through walls)
    EnforcePositionBoundaries(seconds);

    // Update camera position
    usingCamera->MoveTo({position.x, position.y + 1, position.z});

    // Fetch new position bounds if the block the player is in has changed
    UpdateMaxPositions();
}

void Player::EnforcePositionBoundaries(float _seconds) {
    // Check against min and max position values
    if (position.x - radius < minX) {
        position.x = minX + radius;
    }
    if (position.x + radius > maxX) {
        position.x = maxX - radius;
    }
    if (position.z - radius < minZ) {
        position.z = minZ + radius;
    }
    if (position.z + radius > maxZ) {
        position.z = maxZ - radius;
    }
    if (position.y < minY && lastPosition.y > position.y) {
        position.y = minY;
        timeSinceOnGround = 0;
    }
    if (position.y > maxY) {
        position.y = maxY;
    }
}

void Player::FlyingMovement(float _seconds) {
    // max speeds
    float maxHorizSpeed = 15.0f;
    float maxVertSpeed = 15.0f;

    // Determine direction of movement
    moveDirection = {0, 0, 0};
    const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) {
        moveDirection += normalFront;
    }
    if (state[SDL_SCANCODE_S]) {
        moveDirection -= normalFront;
    }
    if (state[SDL_SCANCODE_A]) {
        moveDirection -= normalRight;
    }
    if (state[SDL_SCANCODE_D]) {
        moveDirection += normalRight;
    }
    if (state[SDL_SCANCODE_SPACE]) {
        moveDirection += normalUp;
    }
    if (state[SDL_SCANCODE_LSHIFT]) {
        moveDirection -= normalUp;
    }
    if (state[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 22.5f;
    }
    if (!state[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 15.0f;
    }

    // normalise direction to prevent diagonal movement being too fast
    if (glm::length(moveDirection) > 0) {
        moveDirection = glm::normalize(moveDirection);
    }

    // Set speed value
    vectorSpeed.x = moveDirection.x * maxHorizSpeed;
    vectorSpeed.y = moveDirection.y * maxVertSpeed;
    vectorSpeed.z = moveDirection.z * maxHorizSpeed;

    glm::vec2 horizAccelDir{moveDirection.x, moveDirection.z};
    if (glm::length(horizAccelDir) == 0)
        vectorSpeed = {0.0f, vectorSpeed.y, 0.0f};

    if (glm::length(moveDirection.y) == 0)
        vectorSpeed.y = 0.0f;

    // Limit speed (horiz and vert limited separately)
    glm::vec2 horizSpeed = {vectorSpeed.x, vectorSpeed.z};
    if (glm::length(horizSpeed) > maxHorizSpeed) {
        horizSpeed = glm::normalize(horizSpeed);
        vectorSpeed.x = horizSpeed.x * maxHorizSpeed;
        vectorSpeed.z = horizSpeed.y * maxHorizSpeed;
    }

    // Between +-maxVertSpeed
    vectorSpeed.y = std::min(vectorSpeed.y, maxVertSpeed);
    vectorSpeed.y = std::max(vectorSpeed.y, -maxVertSpeed);

    position += vectorSpeed * _seconds;
}

void Player::WalkingMovement(float _seconds) {
    // accel/decel values are blocks/second
    vectorAcceleration.y = GRAVITY;

    // max speeds
    float maxHorizSpeed = 5.0f;
    float maxVertSpeed = 15.0f;

    // Determine direction of movement
    moveDirection = {0, 0, 0};
    const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) {
        moveDirection += normalFront;
    }
    if (state[SDL_SCANCODE_S]) {
        moveDirection -= normalFront;
    }
    if (state[SDL_SCANCODE_A]) {
        moveDirection -= normalRight;
    }
    if (state[SDL_SCANCODE_D]) {
        moveDirection += normalRight;
    }

    // is player walking or running
    if (state[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 15.0f;
    }
    if (!state[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 10.0f;
    }

    // normalise direction to prevent diagonal movement being too fast
    if (glm::length(moveDirection) > 0) {
        moveDirection = glm::normalize(moveDirection);
    }

    vectorSpeed.x = moveDirection.x * maxHorizSpeed;
    vectorSpeed.z = moveDirection.z * maxHorizSpeed;

    // just walked off ledge, off ground
    if (position.y > minY && timeSinceOnGround == 0) {
        timeSinceOnGround = 0.001;
    }

    // If space is pressed when the player is on solid ground, then start a jump
    if (state[SDL_SCANCODE_SPACE] && timeSinceOnGround == 0) {
        vectorSpeed.y = JUMPSPEED;
        timeSinceOnGround = 0.001;
    }

    // Updating the last stored position
    if (timeSinceOnGround == 0) {
        vectorSpeed.y = 0.0f;
        lastStaticPosition = position;
    }
    else {
        lastStaticPosition = {position.x, lastStaticPosition.y, position.z};
        timeSinceOnGround += _seconds;
    }

    lastPosition = position;

    position = (vectorSpeed * _seconds) + lastStaticPosition;
    position.y = (vectorSpeed.y * timeSinceOnGround) + (0.5f * vectorAcceleration.y * std::pow(timeSinceOnGround, 2.0f)) + lastStaticPosition.y;
}

void Player::UpdateMaxPositions() {
    // Attempt to obtain the chunk of the player
    glm::vec3 chunkPos = (position / (float)chunkSize);
    chunkPos = {chunkPos.x + worldSize / 2.0f, chunkPos.y, chunkPos.z + worldSize/2.0f};
    Chunk* playerChunk = world->GetChunkAtPosition(chunkPos);

    // if a chunk cannot be obtained, player potentially exceeding world area. Force limits of world size
    if (playerChunk == nullptr) {
        minY = -20;
        maxY = worldHeight * chunkSize;
        minX = -(worldSize/2.0f) * chunkSize;
        maxX = (worldSize/2.0f) * chunkSize;
        minZ = -(worldSize/2.0f) * chunkSize;
        maxZ = (worldSize/2.0f) * chunkSize;
    }

    else {
        glm::vec3 playerBlockPos = {chunkPos.x - (int)chunkPos.x, chunkPos.y - (int)chunkPos.y, chunkPos.z - (int)chunkPos.z};
        playerBlockPos *= chunkSize;
        playerBlockPos = {playerBlockPos.x, playerBlockPos.y - 1, playerBlockPos.z};

        // Get the highest ylevel that the player would reach first
        minY = playerChunk->GetTopLevelAtPosition(playerBlockPos, 0.4f);
    }


    maxY = worldHeight * chunkSize;
    minX = -(worldSize/2.0f) * chunkSize;
    maxX = (worldSize/2.0f) * chunkSize;
    minZ = -(worldSize/2.0f) * chunkSize;
    maxZ = (worldSize/2.0f) * chunkSize;
}




void Player::MouseLook(SDL_bool _mouseGrabbed) {
    if (_mouseGrabbed == SDL_FALSE) return;

    // Get dimensions of screen and position of mouse in screen
    int maxx, maxy, mousex, mousey;
    window.GetWindowSize(maxx, maxy);
    SDL_GetMouseState(&mousex, &mousey);

    // Return if no mouse movement made
    if (mousex == maxx/2 && mousey == maxy/2) {
        cameraMoved = false;
        return;
    }
    cameraMoved = true;

    // Offset of mouse from centre
    double xOffset = mousex - (maxx/2.0);
    double yOffset = (maxy/2.0) - mousey;

    // Apply camera sensitivity
    xOffset *= cameraSensitivity;
    yOffset *= cameraSensitivity;

    // Add to current camera angle and apply max angle boundaries
    auto [angleVert, angleHoriz] = usingCamera->GetAngle();
    angleHoriz += xOffset;
    if (angleHoriz > 360) angleHoriz -= 360;
    if (angleHoriz < -360) angleHoriz += 360;

    angleVert += yOffset;
    angleVert = std::min(angleVert, 89.0);
    angleVert = std::max(angleVert, -89.0);

    // Default direction is in positive z axis
    glm::vec3 dirDefault(0.0f, 0.0f, 1.0f);

    // Horizontal Rotation
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),
                                     -(float)glm::radians(angleHoriz),
                                     normalUp);

    // vertical rotation
    rotation = glm::rotate(rotation,
                           (float)glm::radians(angleVert),
                           glm::normalize(glm::cross(dirDefault, normalUp)));

    // Apply rotation
    facingDirection = glm::normalize(rotation * glm::vec4(dirDefault, 1.0f));
    usingCamera->SetDirection(facingDirection);
    usingCamera->SetAngle(angleVert, angleHoriz);

    // Return mouse to centre
    SDL_WarpMouseInWindow(window.WindowPtr(), maxx/2, maxy/2);

    // Update normals for movement
    glm::vec3 horizDirection(facingDirection.x, 0.0f, facingDirection.z);
    normalRight = glm::normalize(glm::cross(horizDirection, normalUp));

    normalFront = glm::normalize(glm::cross(normalUp, normalRight));
}
