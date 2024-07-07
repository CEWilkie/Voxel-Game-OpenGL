#version 410 core

// PIPE IN
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColours;

// PIPE OUT
out vec3 v_vertexColours;

uniform float uOffset;

void main() {
    gl_Position = vec4(position.x, position.y+uOffset, position.z, 1.0f);

    v_vertexColours = vertexColours;
}
