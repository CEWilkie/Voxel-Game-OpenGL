#version 410 core

// PIPE IN
layout(location = 0) in vec3 chunkPosition;
layout(location = 1) in vec3 modelVertex;
layout(location = 2) in float lightLevel;

// UNIFORMS
layout(std140) uniform struct Matricies {
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
    mat4 uModelMatrix;
} matricies;

// PIPE OUT
out float v_lightLevel;

void main() {
    gl_Position = vec4(modelVertex + chunkPosition + vec3(0.5, 0.5, 0.5), 1.0f);
    gl_Position = matricies.uProjectionMatrix * matricies.uViewMatrix * matricies.uModelMatrix * gl_Position;

    v_lightLevel = lightLevel;
}