//
// Created by cew05 on 01/08/2024.
//

#include "Player.h"
#include "../Window.h"

Player::Player(glm::vec3 _position, glm::vec3 _facingDirection) {
    position = _position;
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

    // Movement and input values
    horizSpeed = 30.0f;
    vertSpeed = 20.f;
    cameraSensitivity = 0.1f;
}

void Player::Move(Uint64 _deltaFrames) {
    glm::vec3 horizDirection(facingDirection.x, 0.0f, facingDirection.z);
    normalRight = glm::normalize(glm::cross(horizDirection, normalUp));
    auto [angleVert, angleHoriz] = usingCamera->GetAngle();

    float hs = horizSpeed * float(_deltaFrames) / 1000.0f;
    float vs = vertSpeed * float(_deltaFrames) / 1000.0f;

    const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) {
        position.z += hs * float(cos(glm::radians(angleHoriz)));
        position.x -= hs * float(sin(glm::radians(angleHoriz)));
    }
    if (state[SDL_SCANCODE_S]) {
        position.z -= hs * float(cos(glm::radians(angleHoriz)));
        position.x += hs * float(sin(glm::radians(angleHoriz)));
    }
    if (state[SDL_SCANCODE_A]) {
        position -= hs * normalRight;
    }
    if (state[SDL_SCANCODE_D]) {
        position += hs * normalRight;
    }

    if (state[SDL_SCANCODE_SPACE]) {
        position.y += vs;
    }
    if (state[SDL_SCANCODE_LSHIFT]) {
        position.y -= vs;
    }

    usingCamera->MoveTo({position.x, position.y + 1, position.z});
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

    // Apply rotation and return mouse to centre
    facingDirection = glm::normalize(rotation * glm::vec4(dirDefault, 1.0f));
    usingCamera->SetDirection(facingDirection);
    usingCamera->SetAngle(angleVert, angleHoriz);
    SDL_WarpMouseInWindow(window.WindowPtr(), maxx/2, maxy/2);
}
