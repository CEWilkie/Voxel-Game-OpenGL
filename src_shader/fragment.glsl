#version 410 core

// PIPE IN
in vec3 v_vertexColour;
in vec2 v_vertexTextureCoord;

uniform sampler2D tex0;

// PIPE OUT
out vec4 color;

void main() {
    color = texture(tex0, v_vertexTextureCoord);

    if (color.a == 0.0f) color = vec4(v_vertexColour.r, v_vertexColour.g, v_vertexColour.b, 1.0f);
}