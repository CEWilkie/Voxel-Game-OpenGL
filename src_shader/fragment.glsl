#version 410 core

// PIPE IN
in vec2 v_vertexTextureCoord;

// UNIFORMS
uniform sampler2D tex0;

layout(std140) uniform struct WorldAmbients {
    float lightingStrength;
    float minFogDistance;
    float maxFogDistance;
} worldAmbients;

// PIPE OUT
out vec4 color;

void main() {
    // Fetch block texture
    color = texture(tex0, v_vertexTextureCoord);

    // Apply ambient lighting
    color = vec4(vec3(color.r, color.g, color.b) * worldAmbients.lightingStrength, color.a);

    // ...

}