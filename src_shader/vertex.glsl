#version 410 core

// PIPE IN
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColour;
layout(location = 2) in vec2 vertexTextureCoord;

// PIPE OUT
out vec3 v_vertexColour;
out vec2 v_vertexTextureCoord;

// UNIFORMS
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

void main() {
    // Turn position into vec4
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);

    // Apply transformation matrix
    gl_Position = uModelMatrix * gl_Position;

    // Apply view matrix
    gl_Position = uViewMatrix * gl_Position;

    // Projection
    gl_Position = uProjectionMatrix * gl_Position;


    // Colour
    v_vertexColour = vertexColour;

    // Texture
    v_vertexTextureCoord = vertexTextureCoord;
}
