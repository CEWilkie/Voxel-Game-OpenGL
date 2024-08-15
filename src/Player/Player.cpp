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

    usingCamera->UpdateViewFrustrum();
    usingCamera->UpdateLookatUniform();

    cameraSensitivity = 0.1f;

    // Set initial values for min max positions
    minY = -20;
    maxY = worldHeight * chunkSize;
    minX = -(worldSize/2.0f) * chunkSize;
    maxX = (worldSize/2.0f) * chunkSize;
    minZ = -(worldSize/2.0f) * chunkSize;
    maxZ = (worldSize/2.0f) * chunkSize;

    UpdateMaxPositions();
}


void Player::Display() {
    // Furthest distance a ray from player can travel before hitting interactable object
    if (playerChunk == nullptr) return;

    // Get block at rayPosition
    Block* targetBlock = playerChunk->GetBlockAtPosition(unobstructedRayPosition, 0);
    if (targetBlock != nullptr) {
        Transformation t;
        glm::vec3 pos = glm::floor(unobstructedRayPosition) + playerChunk->GetPosition() * (float)chunkSize;
        t.SetPosition(pos);
        t.UpdateModelMatrix();
        targetBlock->DisplayWireframe(&t);
    }
}

void Player::HandleMovement(Uint64 _deltaTicks) {
    // Seconds that have passed since last movement update
    float seconds = float(_deltaTicks) / 1000.0f;

    // Update player position using correct movement method
    GetMovementFriction();
    const std::uint8_t* keyInputs = SDL_GetKeyboardState(nullptr);
    switch (movementMode) {
        case MOVEMENTMODE::FLYING:
            FlyingMovement(keyInputs, seconds);
            break;

        default: // WALKING
            WalkingMovement(keyInputs, seconds);
    }

    // Ensure that the new position remains within the boundaries (not travel through walls)
    EnforcePositionBoundaries(seconds);

    // Check for the player attempting to switch camera
    SwitchCamera(keyInputs);

    // Update camera position
    if (currentCamera == 1) {
        usingCamera->MoveTo({position.x, position.y + 1, position.z});
    }
    else if (currentCamera == 3) {
        glm::vec3 thirdPersonPos = glm::vec3(position.x, position.y + 1, position.z) + normalUp - normalFront*3.0f;
        usingCamera->MoveTo(thirdPersonPos);
    }
    usingCamera->UpdateViewFrustrum();

    // Fetch new position bounds if the block the player is in has changed
    if (lastPosition != position) { UpdatePlayerChunk(); }
    UpdateMaxPositions();

    GetUnobstructedRayPosition();
}

void Player::FlyingMovement(const std::uint8_t* _keyInputs, float _seconds) {
    // max speeds
    float maxHorizSpeed = 15.0f;
    float maxVertSpeed = 15.0f;

    // Determine direction of movement
    moveDirection = {0, 0, 0};
    if (_keyInputs[SDL_SCANCODE_W]) {
        moveDirection += normalFront;
    }
    if (_keyInputs[SDL_SCANCODE_S]) {
        moveDirection -= normalFront;
    }
    if (_keyInputs[SDL_SCANCODE_A]) {
        moveDirection -= normalRight;
    }
    if (_keyInputs[SDL_SCANCODE_D]) {
        moveDirection += normalRight;
    }
    if (_keyInputs[SDL_SCANCODE_SPACE]) {
        moveDirection += normalUp;
    }
    if (_keyInputs[SDL_SCANCODE_LSHIFT]) {
        moveDirection -= normalUp;
    }
    if (_keyInputs[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 22.5f;
    }
    if (!_keyInputs[SDL_SCANCODE_LCTRL]) {
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

void Player::WalkingMovement(const std::uint8_t* _keyInputs, float _seconds) {
    // accel/decel values are blocks/second
    vectorAcceleration.y = GRAVITY;

    // max speeds
    float maxHorizSpeed;

    // Determine direction of movement
    moveDirection = {0, 0, 0};
    if (_keyInputs[SDL_SCANCODE_W]) {
        moveDirection += normalFront;
    }
    if (_keyInputs[SDL_SCANCODE_S]) {
        moveDirection -= normalFront;
    }
    if (_keyInputs[SDL_SCANCODE_A]) {
        moveDirection -= normalRight;
    }
    if (_keyInputs[SDL_SCANCODE_D]) {
        moveDirection += normalRight;
    }

    if (_keyInputs[SDL_SCANCODE_C]) {
        printf("PLAYERPOS: %f %f %f\n", position.x, position.y, position.z);
    }

    // is player walking or running
    if (_keyInputs[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 7.5f;
    }
    if (!_keyInputs[SDL_SCANCODE_LCTRL]) {
        maxHorizSpeed = 4.5f;
    }

    // normalise direction to prevent diagonal movement being too fast
    if (glm::length(moveDirection) > 0) {
        moveDirection = glm::normalize(moveDirection);
    }

    vectorSpeed.x = moveDirection.x * maxHorizSpeed;
    vectorSpeed.z = moveDirection.z * maxHorizSpeed;

    // Updating the last static position before determining if a movement has started
    if (timeSinceOnGround == 0) {
        vectorSpeed.y = 0.0f;
        lastStaticPosition = position;
    }
    else {
        lastStaticPosition = {position.x, lastStaticPosition.y, position.z};
        timeSinceOnGround += _seconds;
    }

    // just walked off ledge, off ground
    if (position.y > minY && timeSinceOnGround == 0) {
        timeSinceOnGround = _seconds;
        canJump = false;
    }

    // If space is pressed when the player is on solid ground or in liquid, then start a jump
    if (_keyInputs[SDL_SCANCODE_SPACE] && canJump) {
        if (inLiquid) {
            vectorSpeed.y = JUMPSPEED * 0.7f;
        }
        else {
            vectorSpeed.y = JUMPSPEED;
        }
        timeSinceOnGround = _seconds;
        lastStaticPosition.y = position.y;
        canJump = false;
    }

    lastPosition = position;

    position = (vectorSpeed * _seconds) + lastStaticPosition;
    position.y = (vectorSpeed.y * timeSinceOnGround) + (0.5f * vectorAcceleration.y * std::pow(timeSinceOnGround, 2.0f)) + lastStaticPosition.y;
}

void Player::SwitchCamera(const std::uint8_t* _keyInputs) {
    if (_keyInputs[SDL_SCANCODE_R]) {
        if (!camSwitchToggle) {
            if (currentCamera == 1) {
                usingCamera = thirdPerson.get();
                currentCamera = 3;
            }
            else if (currentCamera == 3) {
                usingCamera = firstPerson.get();
                currentCamera = 1;
            }

        }

        camSwitchToggle = true;
    }

    if (!_keyInputs[SDL_SCANCODE_R]) camSwitchToggle = false;
}

void Player::UpdatePlayerChunk() {
    // Attempt to obtain the chunk of the player, this may be null

    glm::vec3 chunkPos = (position / (float)chunkSize);
    chunkPos = {chunkPos.x + worldSize / 2.0f, chunkPos.y, chunkPos.z + worldSize/2.0f};
    playerChunk = world->GetChunkAtPosition(chunkPos);
}


void Player::UpdateMaxPositions() {
    // if a chunk was not obtained, dont update the previous min max values
    if (playerChunk == nullptr) return;

    // Get block position in chunk player is currently inside of
    glm::vec3 blockPos = position - (playerChunk->GetPosition() * (float)chunkSize);

    // Get the highest ylevel that the player would reach first
    minY = playerChunk->GetTopLevelAtPosition({blockPos.x, blockPos.y - 1, blockPos.z}, 0.4f);
    maxY = worldHeight * chunkSize;

    // returns position of obstructing face in the chunk
    minX = (playerChunk->GetPosition().x * (float)chunkSize) + playerChunk->GetDistanceToBlockFace(blockPos, dirFront, radius);
    maxX = (playerChunk->GetPosition().x * (float)chunkSize) + playerChunk->GetDistanceToBlockFace(blockPos, dirBack, radius);

    minZ = (playerChunk->GetPosition().z * (float)chunkSize) + playerChunk->GetDistanceToBlockFace(blockPos, dirLeft, radius);
    maxZ = (playerChunk->GetPosition().z * (float)chunkSize) + playerChunk->GetDistanceToBlockFace(blockPos, dirRight, radius);
}

void Player::EnforcePositionBoundaries(float _seconds) {
    // Check against min and max position values
    if (position.x - radius < minX) {
        position.x = minX + radius;
    }
    else if (position.x + radius > maxX) {
        position.x = maxX - radius;
    }

    if (position.z - radius < minZ) {
        position.z = minZ + radius;
    }
    else if (position.z + radius > maxZ) {
        position.z = maxZ - radius;
    }

    if (position.y < minY && lastPosition.y > position.y) {
        position.y = minY;
        timeSinceOnGround = 0;
        canJump = true;
    }
    else if (position.y > maxY) {
        position.y = maxY;
    }
}

void Player::GetMovementFriction() {
    // Get block position in chunk player is currently inside of
    if (playerChunk == nullptr) return;
    glm::vec3 blockPos = position - (playerChunk->GetPosition() * (float) chunkSize);

    Block *playerBlock = playerChunk->GetBlockAtPosition(blockPos, 0);
    if (playerBlock == nullptr) return;

    // If player is in a liquid then reset jump time
    inLiquid = false;
    if (playerBlock->GetAttributeValue(BLOCKATTRIBUTE::LIQUID) > 0) {
        canJump = true;
        inLiquid = true;
    }
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

    GetUnobstructedRayPosition();
}


void Player::HandlePlayerInputs(const SDL_Event &_event) {
    if (playerChunk == nullptr) return;

    if (_event.type == SDL_MOUSEBUTTONDOWN) {
        switch (_event.button.button) {
            case SDL_BUTTON_LEFT:
                BreakBlock(unobstructedRayPosition);
                break;

            case SDL_BUTTON_RIGHT:
                PlaceBlock(unobstructedRayPosition);
                break;
        }
    }
}

void Player::GetUnobstructedRayPosition() {
    if (playerChunk == nullptr) return;

    // Create ray from campos using direction and max range, detect first block ray encounters that can be interacted
    // with
    glm::vec3 rayPosition = firstPerson->GetPosition() - (playerChunk->GetPosition() * (float)chunkSize);
    rayPosition.y += 1;

    // higher accuracy -> more checks, smaller increments in position along the line
    float accuracy = 20.0f;

    for (int r = 0; r < (int)accuracy; r++) {
        Block* blockAtPosition = playerChunk->GetBlockAtPosition(rayPosition, 0);
        if (blockAtPosition != nullptr) {
            // if the block is breakable, end early
            if (blockAtPosition->GetAttributeValue(BLOCKATTRIBUTE::BREAKABLE) > 0) {
                break;
            }

            // block not breakable, however only proceed if the player can access through the block
            if (blockAtPosition->GetAttributeValue(BLOCKATTRIBUTE::CANACCESSTHROUGHBLOCK) == 0) {
                break;
            }
        }

        // next position
        rayPosition += glm::normalize(facingDirection) * (range/accuracy);
    }

    unobstructedRayPosition = rayPosition;
}

void Player::BreakBlock(glm::vec3 _rayPosition) {
    Block* blockAtPosition = playerChunk->GetBlockAtPosition(_rayPosition, 0);
    if (blockAtPosition == nullptr) return;

    // else a block has been found, so break it
    playerChunk->BreakBlockAtPosition(_rayPosition);
}

void Player::PlaceBlock(glm::vec3 _rayPosition) {
    glm::vec3 rayPosition = usingCamera->GetPosition() - (playerChunk->GetPosition() * (float)chunkSize);

    playerChunk->PlaceBlockAtPosition(rayPosition, {STONE, 0});
}