#version 410 core

// PIPE IN
in vec2 v_vertexTextureCoord;

uniform sampler2D tex0;

// PIPE OUT
out vec4 color;

void main() {
    color = texture(tex0, v_vertexTextureCoord);

    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

    color = vec4(vec3(color.r, color.g, color.b) * ambient, 1.0f);
}