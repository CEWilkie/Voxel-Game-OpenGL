#version 410 core

// PIPE IN
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColours;

// PIPE OUT
out vec3 v_vertexColours;

// UNIFORMS
uniform mat4 uTranslationMatrix;
uniform mat4 uRotationMatrix;
uniform mat4 uProjectionMatrix;

void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);

    gl_Position = uTranslationMatrix * gl_Position;
    gl_Position = uRotationMatrix * gl_Position;
    gl_Position = uProjectionMatrix * gl_Position;

    v_vertexColours = vertexColours;
}
