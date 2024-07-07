//
// Created by cew05 on 07/07/2024.
//

#include "Quad.h"

//
// Created by cew05 on 16/04/2024.
//

#include "Triangle.h"

#include <cmath>
#include <random>
#include <algorithm>

Quad::Quad() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &colorBufferObject);

    // Set random speeds
    std::random_device rd;
    std::mt19937 mt(rd());
    vel.first = float((int(mt()) % 10) / 1000.0);
    vel.second = float((int(mt()) % 10) / 1000.0);

    // Triangle position, min distance from centre
    auto cx = float((int(mt()) % 10) / 20.0);
    auto cy = float((int(mt()) % 10) / 20.0);
    float r = 0.5;
    numAttribs = 3;

    // Create vertex positions (two triangles
    vertexArray = {
            // bottom Left Triangle
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
            // Top Right Triangle
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
    };

    // Now create a set of colours for the verticies
    vertexColorArray = {
            // Bottom left triangle
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            // Top right triangle
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };
}

Quad::~Quad() {
    // Deallocate texture
    glDeleteTextures(1, &textureObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &colorBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void Quad::ConstructQuad() {
    // bind object id
    glBindVertexArray(vertexArrayObject);

    // bind vertex buffer array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, numAttribs, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*numAttribs, nullptr);

    // Bind colour buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexColorArray.size() * sizeof(float)), vertexColorArray.data(), GL_STATIC_DRAW);

    // Vertex Colour Attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*3, nullptr);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void Quad::Move() {
    // bind the vertex buffer as data will be changed
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    for (int v = 0; v < vertexArray.size(); v += numAttribs) {
        // x, y, [optional][z] [...]
        vertexArray[v] += vel.first;
        vertexArray[v+1] += vel.second;
    }

    // Check border collision
    for (int v = 0; v < vertexArray.size(); v += numAttribs) {
        if (vertexArray[v] >= 1 || vertexArray[v] <= -1) vel.first *= -1;
        if (vertexArray[v+1] >= 1 || vertexArray[v+1] <= -1) vel.second *= -1;
    }

    // Update buffer data and close binding
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::Display() const {
    // Bind
//    glBindTexture(GL_TEXTURE_2D, textureObject);
//    glEnable(GL_TEXTURE_2D);
    glBindVertexArray(vertexArrayObject);

    // Enable Attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Draw
//    glBegin(GL_TRIANGLES);
//    glTexCoord2f(0.0f, 0.0f);
//    glVertex3f(vertexArray[0], vertexArray[1], vertexArray[2]);
//    glTexCoord2f(1.0f, 0.0f);
//    glVertex3f(vertexArray[3], vertexArray[4], vertexArray[5]);
//    glTexCoord2f(0.0f, 1.0f);
//    glVertex3f(vertexArray[6], vertexArray[7], vertexArray[8]);
//    glEnd();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind
    glBindVertexArray(0);

    // Disable Attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


//    glBindTexture(GL_TEXTURE_2D, 0);
//    glDisable(GL_TEXTURE_2D);
}

