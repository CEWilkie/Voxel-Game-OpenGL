#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColours;

out vec3 v_vertexColours;

void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);

    v_vertexColours = vertexColours;
}
