#version 410 core

// PIPE IN
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexTextureCoord;

// PIPE OUT
out vec2 v_vertexTextureCoord;

// UNIFORMS
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

uniform vec2 uVertexTextureCoordOffset = vec2(0.0f, 0.0f);



void main() {
    // Turn position into vec4
    gl_Position = vec4(position.x, position.y, position.z, 1.0f);

    // Apply transformation matrix
    gl_Position = uModelMatrix * gl_Position;

    // Apply view matrix
    gl_Position = uViewMatrix * gl_Position;

    // Projection
    gl_Position = uProjectionMatrix * gl_Position;

    // Determine true texture coordinates for 16x16 texture grids
    v_vertexTextureCoord = vec2(uVertexTextureCoordOffset.x + vertexTextureCoord.x,
                                uVertexTextureCoordOffset.y + vertexTextureCoord.y) / 16.0f;

}
