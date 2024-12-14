#version 410 core

/*
 * PIPE IN
 */

// Positioning
layout(location = 0) in vec3 vertexOffsetInChunk;
layout(location = 1) in vec3 vertexInModel;
layout(location = 2) in vec3 blockFaceAxis;

// Texture and Lighting
layout(location = 3) in vec2 vertexTextureCoord;
layout(location = 4) in vec2 blockRotation;
layout(location = 5) in float vertexOcclusion;
layout(location = 6) in float vertexLightLevel;


//layout(location = 3) in vec2 blockRotation;

// PIPE OUT
out float v_vertexDistanceFromCamera;
out float v_vertexOcclusion;
out float v_vertexLightLevel;
out vec2 v_vertexTextureCoord;
out vec4 v_vertexTextureColorOverride;

// UNIFORMS
uniform struct Matricies {
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
    mat4 uModelMatrix;
} matricies;

uniform int uBlockModel = 0;
uniform vec2 uVertexTextureCoordOffset = vec2(0.0f, 0.0f);
uniform vec4 vertexTextureColorOverride = vec4(0.0f, 0.0f, 0.0f, 0.0f);

float GetVertexDistance();
mat3 RotationMatrix(float _angleRads, vec3 _axis);
mat2 RotateTexture(float _angleRads);
vec2 BlockFaceTextureOffset(vec3 _faceAxis);

void main() {
    // update vertex to position in chunk, then apply Projection, view and model matricies
    gl_Position = vec4(vertexInModel + vertexOffsetInChunk, 1.0f);
    gl_Position = matricies.uProjectionMatrix * matricies.uViewMatrix * matricies.uModelMatrix * gl_Position;



    /*
     * Rotate the face axis of the block to determine the true face texture coords
     */

    // Offset to centralise around 0,0
    vec2 texCoord = vertexTextureCoord - vec2(0.5f, 0.5f);

    // Rotate only top face texture coords for now
    if (blockFaceAxis.y == 1) {
        float angleDeg = float(blockRotation.x) * 90.0f;
        texCoord = (RotateTexture(radians(angleDeg))) * texCoord;
    }

    // Remove offset around 0,0 to realign with textureSheet Grid
    texCoord += vec2(0.5f, 0.5f);

    // Retrieve actual block face texture coords based upon face axis. Apply offset of origin of texture in
    // texture sheet
    texCoord += BlockFaceTextureOffset(blockFaceAxis);
    v_vertexTextureCoord = vec2(uVertexTextureCoordOffset.x + texCoord.x,
                                uVertexTextureCoordOffset.y + texCoord.y) / 16.0f;


    /*
     * PIPE OUT
     */

    // Pass distance from camera to vertex into frag shader
    v_vertexDistanceFromCamera = GetVertexDistance();

    // If this value's alpha is not 0, the colour is overlay onto the texture
    v_vertexTextureColorOverride = vertexTextureColorOverride;

    // vertex Occlusion
    float occlusion = (vertexOcclusion != 0) ? 0.75 : 1;
    v_vertexOcclusion = occlusion;

    // Vertex Light level
    float light = vertexLightLevel / 15.0f;
    light = clamp(light, 0.1, 1);
    v_vertexLightLevel = light;
}


float GetVertexDistance() {
    // distance from the camera position to the vertex at gl_Position
    vec3 campos = matricies.uViewMatrix[0].xyz;
    return distance(campos, gl_Position.xyz);
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




mat2 RotateTexture(float _angleRads) {
    float s = sin(_angleRads);
    float c = cos(_angleRads);
    return mat2(
            c, -s,
            s, c
    );
}



vec2 BlockFaceTextureOffset(vec3 _faceAxis) {
    if (uBlockModel == 1) { // FULL BLOCK
        if (_faceAxis.x != 0) return vec2(_faceAxis.x + 1, 0);
        if (_faceAxis.y != 0) return vec2(0, -_faceAxis.y);
        if (_faceAxis.z != 0) return vec2(_faceAxis.z, 0);
    }

    else if (uBlockModel == 2) { // PLANT
        return vec2(0,0);
    }

    return vec2(0,0);
}