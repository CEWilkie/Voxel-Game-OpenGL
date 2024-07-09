//
// Created by cew05 on 08/07/2024.
//

#include "Camera.h"

#include "Window.h"
#include <glew.h>

Camera::Camera() {
    // Set the perspective of the camera
    perspective = glm::perspective(glm::radians(45.0f), window.GetAspectRatio(), 0.1f, 10.0f);

    // Set camera info
    position = glm::vec3(1.7f, 0.0f, 4.8f);
    direction = glm::vec3(0.0f, 0.0f, 1.0f);
    normalUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Set perspective
    GLint uLocation = glGetUniformLocation(window.GetShader(), "uProjectionMatrix");
    if (uLocation < 0) printf("location not found [uProjectionMatrix]");
    else {
        glUniformMatrix4fv(uLocation, 1, GL_FALSE, &perspective[0][0]);
    }
}

void Camera::MoveTo(const glm::vec3 &_position) {
    // set new position
    position = _position;
//    printf("POS: %f %f %f\n", position.x, position.y, position.z);
}

void Camera::Move(Uint64 _deltaFrames) {
    printf("POS: %f %f %f\n", position.x, position.y, position.z);
    printf("FACING DIRECTION: %f %f %f\n", direction.x, direction.y, direction.z);
    printf("ANGLES: %f %f\n\n", angleVert, angleHoriz);

    float spd = float(_deltaFrames)/120.0f;

    glm::vec3 horizDirection(direction.x, 0.0f, direction.z);

    const std::uint8_t* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) {
        position.z += spd * float(cos(glm::radians(angleHoriz)));
        position.x -= spd * float(sin(glm::radians(angleHoriz)));
    }
    if (state[SDL_SCANCODE_S]) {
        position.z -= spd * float(cos(glm::radians(angleHoriz)));
        position.x += spd * float(sin(glm::radians(angleHoriz)));
    }
    if (state[SDL_SCANCODE_A]) {
        position -= spd * glm::normalize(glm::cross(horizDirection, normalUp));
    }
    if (state[SDL_SCANCODE_D]) {
        position += spd * glm::normalize(glm::cross(horizDirection, normalUp));
    }

    position.y = 0;


}

void Camera::MouseLook(SDL_bool _mouseGrabbed) {
    if (_mouseGrabbed == SDL_FALSE) return;

    // Get dimensions of screen and position of mouse in screen
    int maxx, maxy, mousex, mousey;
    window.GetWindowSize(maxx, maxy);
    SDL_GetMouseState(&mousex, &mousey);

    // Return if no mouse movement made
    if (mousex == maxx/2 && mousey == maxy/2) return;

    // Offset of mouse from centre
    double xOffset = mousex - (maxx/2.0);
    double yOffset = (maxy/2.0) - mousey;

    // Apply camera sensitivity
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    // Add to camera angle and apply bounds to Vertical angle
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
    direction = glm::normalize(rotation * glm::vec4(dirDefault, 1.0f));
    SDL_WarpMouseInWindow(window.WindowPtr(), maxx/2, maxy/2);
}

void Camera::UpdateUniform() const {
    GLint uLocation;

    uLocation = glGetUniformLocation(window.GetShader(), "uViewMatrix");
    if (uLocation < 0) printf("location not found [uViewMatrix]");
    else {
        glUniformMatrix4fv(uLocation, 1, GL_FALSE, &GetViewMatrix()[0][0]);
    }
}