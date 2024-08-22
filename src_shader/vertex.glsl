#version 410 core

// PIPE IN
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTextureCoord;

// PIPE OUT
out float v_vertexDistance;
out vec2 v_vertexTextureCoord;
out vec4 v_vertexTextureColorOverride;

// UNIFORMS
layout(std140) uniform struct Matricies {
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
    mat4 uModelMatrix;
} matricies;

uniform vec2 uVertexTextureCoordOffset = vec2(0.0f, 0.0f);
uniform vec4 vertexTextureColorOverride = vec4(0.0f, 0.0f, 0.0f, 0.0f);

float GetVertexDistance() {
    // distance from the camera position to the vertex at gl_Position
    vec3 campos = matricies.uViewMatrix[0].xyz;
    return distance(campos, gl_Position.xyz);
}

void main() {
    // Turn position into vec4 and apply Projection, view and model matricies
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f);
    gl_Position = matricies.uProjectionMatrix * matricies.uViewMatrix * matricies.uModelMatrix * gl_Position;

    // Pass distance from camera to vertex into frag shader
    v_vertexDistance = GetVertexDistance();

    // Determine true texture coordinates for 16x16 texture grids
    v_vertexTextureCoord = vec2(uVertexTextureCoordOffset.x + vertexTextureCoord.x,
                                uVertexTextureCoordOffset.y + vertexTextureCoord.y) / 16.0f;

    // If this value's alpha is not 0, the colour is overlay onto the texture
    v_vertexTextureColorOverride = vertexTextureColorOverride;
}
