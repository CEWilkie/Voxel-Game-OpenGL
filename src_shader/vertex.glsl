#version 410 core

// PIPE IN
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColours;

// PIPE OUT
out vec3 v_vertexColours;

// UNIFORMS
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main() {
    // Turn position into vec4
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);

    // Apply view matrix
    gl_Position = uViewMatrix * gl_Position;

    // Projection
    gl_Position = uProjectionMatrix * gl_Position;

    // Colour
    v_vertexColours = vertexColours;
}
