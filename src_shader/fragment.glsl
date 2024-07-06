#version 410 core

in vec3 v_vertexColours;
layout(location = 0) out vec4 color;

void main() {
    color = vec4(v_vertexColours.r, v_vertexColours.g, v_vertexColours.b, 1.0f);
}