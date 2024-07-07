#version 410 core

// PIPE IN
in vec3 v_vertexColours;

// PIPE OUT
out vec4 color;

void main() {
    color = vec4(v_vertexColours.r, v_vertexColours.g, v_vertexColours.b, 1.0f);
}