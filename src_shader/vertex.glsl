#version 410 core

// PIPE IN
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTextureCoord;
layout(location = 2) in vec3 originVertexPosition;
layout(location = 3) in vec2 blockRotation;

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

mat3 rotation3dY(float angle) {
    float s = sin(angle);
    float c = cos(angle);

    return mat3(
        c, 0.0, -s,
        0.0, 1.0, 0.0,
        s, 0.0, c
    );
}

// Takes a normalised axis of x/y/z direction and produces a matrix to
// rotate a vector position at 0,0 by the given angle

mat3 RotationMatrix(float _angleRads, vec3 _axis) {
    float s = sin(_angleRads);
    float c = cos(_angleRads);

    // x-axis rotation
    if (_axis.x == 1) return mat3(
        1, 0, 0,
        0, c, -s,
        0, s, c
    );

    // y-axis rotation
    if (_axis.y == 1) return mat3(
        c, 0, -s,
        0, 1, 0,
        s, 0, c
    );

    // z-axis rotation
    if (_axis.z == 1) return mat3(
        c, -s, 0,
        s, c, 0,
        0, 0, 1
    );
}

void main() {
    // retrieve and apply block rotation around y-axis
    float angleDeg = float(blockRotation.x) * 90.0f;
    vec3 originVertex = originVertexPosition - vec3(0.5, 0.5, 0.5);
    vec3 rotatedOriginPos = (RotationMatrix(radians(angleDeg), vec3(0,1,0)) * originVertex) + vec3(0.5, 0.5, 0.5);

    // update vertex to position in chunk, then apply Projection, view and model matricies
    gl_Position = vec4(rotatedOriginPos + vertexPosition, 1.0f);
    gl_Position = matricies.uProjectionMatrix * matricies.uViewMatrix * matricies.uModelMatrix * gl_Position;

    // Pass distance from camera to vertex into frag shader
    v_vertexDistance = GetVertexDistance();

    // Determine true texture coordinates for 16x16 texture grids
    v_vertexTextureCoord = vec2(uVertexTextureCoordOffset.x + vertexTextureCoord.x,
                                uVertexTextureCoordOffset.y + vertexTextureCoord.y) / 16.0f;

    // If this value's alpha is not 0, the colour is overlay onto the texture
    v_vertexTextureColorOverride = vertexTextureColorOverride;
}
