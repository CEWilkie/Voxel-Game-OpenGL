#version 410 core

// PIPE IN
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTextureCoord;
layout(location = 2) in vec3 originVertexPosition;
layout(location = 3) in vec2 blockRotationIndex;

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

// Kindly yoinked, stolen, theiveried and nabbed from this lovely site
// https://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
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

void main() {
    // turn position into vec4
    float angleDeg = float(blockRotationIndex.x) * 90.0f;
    vec3 rotatedOriginPos = rotation3dY(radians(angleDeg)) * originVertexPosition;

    if (angleDeg == 90.0f) {
        rotatedOriginPos.z += 1;
    }
    else if (angleDeg == 180.0f) {
        rotatedOriginPos.z += 1;
        rotatedOriginPos.x += 1;
    }
    else if (angleDeg == 270.0f) {
        rotatedOriginPos.x += 1;
    }

    gl_Position = vec4(rotatedOriginPos + vertexPosition, 1.0f);

    // apply Projection, view and model matricies
    gl_Position = matricies.uProjectionMatrix * matricies.uViewMatrix * matricies.uModelMatrix * gl_Position;


    // Pass distance from camera to vertex into frag shader
    v_vertexDistance = GetVertexDistance();

    // Determine true texture coordinates for 16x16 texture grids
    v_vertexTextureCoord = vec2(uVertexTextureCoordOffset.x + vertexTextureCoord.x,
                                uVertexTextureCoordOffset.y + vertexTextureCoord.y) / 16.0f;

    // If this value's alpha is not 0, the colour is overlay onto the texture
    v_vertexTextureColorOverride = vertexTextureColorOverride;
}
