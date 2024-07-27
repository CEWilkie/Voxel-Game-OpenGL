#version 410 core

// PIPE IN
in vec2 v_vertexTextureCoord;

uniform sampler2D tex0;

// PIPE OUT
out vec4 color;

void main() {
    color = texture(tex0, v_vertexTextureCoord);

    if (color.w < 1) color.a = 1.0f;
}