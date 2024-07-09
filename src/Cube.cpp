//
// Created by cew05 on 07/07/2024.
//

#include "Cube.h"

#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>

Cube::Cube() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &colorBufferObject);
    glGenBuffers(1, &indexBufferObject);

    // Create vertex array for cube
    vertexArray = {
            // Top level
            0.0f, 0.0f, 0.0f,               // BOTTOMLEFT VERTEX
            0.5f, 0.0f, 0.0f,               // BOTTOMRIGHT VERTEX
            0.0f, 0.0f, 0.5f,               // TOPLEFT VERTEX
            0.5f, 0.0f, 0.5f,               // TOPRIGHT VERTEX
            // Bottom level
            0.0f, -0.5f, 0.0f,               // BOTTOMLEFT VERTEX
            0.5f, -0.5f, 0.0f,               // BOTTOMRIGHT VERTEX
            0.0f, -0.5f, 0.5f,               // TOPLEFT VERTEX
            0.5f, -0.5f, 0.5f,               // TOPRIGHT VERTEX
    };

    // Create normals to x, y, z planes
    xNorm = glm::cross(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f));
    yNorm = glm::cross(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f));
    zNorm = glm::cross(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f));


    // Create index buffer for traversal order to produce each cube face
    indexArray = {
            // LEFTSIDEFACE
            6, 4, 2,
            4, 0, 2,
            // BOTTOMFACE
            4, 5, 6,
            5, 7, 6,
            // BACKFACE
            7, 2, 3,
            7, 6, 2,
            // TOPFACE
            0, 1, 2,
            1, 3, 2,
            // FRONTFACE
            4, 5, 0,
            5, 1, 0,
            // RIGHTSIDEFACE
            5, 7, 1,
            7, 3, 1,
    };

    // Create colour array
    vertexColorArray = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    BindCube();
}

Cube::~Cube() {
    printf("Cube Destroyed\n");

    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &colorBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}


void Cube::BindCube() {
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*3, nullptr);

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexArray.size()*sizeof(GLuint)), indexArray.data(), GL_STREAM_DRAW);

    // Bind colour buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexColorArray.size() * sizeof(float)), vertexColorArray.data(), GL_STATIC_DRAW);

    // Vertex Colour Attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*3, nullptr);

    // Unbind arrays / buffers
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR) {
        printf("GL ERROR CODE %u STRING : %s\n", e, glewGetErrorString(e));
    }
}


void Cube::Display() const {
    // Bind object
    glBindVertexArray(vertexArrayObject);

    // Enable Attributes
    glEnable(GL_DEPTH_TEST);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLCHECK(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

    // unbind
    glDisable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Cube::SetPosition(const std::vector<float> &_originVertex) {
    vertexArray = {
            // Top level
            _originVertex[0]        , _originVertex[1]          , _originVertex[2]          ,               // BOTTOMLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1]          , _originVertex[2]          ,               // BOTTOMRIGHT VERTEX
            _originVertex[0]        , _originVertex[1]          , _originVertex[2] + 0.5f   ,               // TOPLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1]          , _originVertex[2] + 0.5f   ,               // TOPRIGHT VERTEX
            // Bottom level
            _originVertex[0]        , _originVertex[1] - 0.5f   , _originVertex[2]          ,               // BOTTOMLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1] - 0.5f   , _originVertex[2]          ,               // BOTTOMRIGHT VERTEX
            _originVertex[0]        , _originVertex[1] - 0.5f   , _originVertex[2] + 0.5f   ,               // TOPLEFT VERTEX
            _originVertex[0] + 0.5f , _originVertex[1] - 0.5f   , _originVertex[2] + 0.5f   ,               // TOPRIGHT VERTEX
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::Rotate(const std::vector<float>& _theta) {
    // Get rotation matrix
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), _theta[0], xNorm);
    rot = glm::rotate(rot, _theta[1], yNorm);
    rot = glm::rotate(rot, _theta[2], zNorm);

    GLint rmLocation = glGetUniformLocation(window.GetShader(), "uRotationMatrix");
    if (rmLocation < 0) printf("location not found [uRotationMatrix]");
    else {
        glUniformMatrix4fv(rmLocation, 1, GL_FALSE, &rot[0][0]);
    }
}

void Cube::Move(const std::vector<float>& _dist) {
    glm::vec3 vec(_dist[0], _dist[1], _dist[2]);
    glm::mat4 move = glm::translate(glm::mat4(1.0f), vec);

    GLint rmLocation = glGetUniformLocation(window.GetShader(), "uTranslationMatrix");
    if (rmLocation < 0) printf("location not found [uTranslationMatrix]");
    else {
        glUniformMatrix4fv(rmLocation, 1, GL_FALSE, &move[0][0]);
    }
}