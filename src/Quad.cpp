//
// Created by cew05 on 07/07/2024.
//

#include "Quad.h"

Quad::Quad() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &colorBufferObject);
    glGenBuffers(1, &indexBufferObject);

    // Set random speeds
    std::random_device rd;
    std::mt19937 mt(rd());
    vel.first = float((int(mt()) % 10) / 1000.0);
    vel.second = float((int(mt()) % 10) / 1000.0);

    // Triangle position, min distance from centre
    numAttribs = 3;

    // Create vertex positions (two triangles
    vertexArray = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
    };

    // index array to determine order verticies are traversed when drawing
    indexBufferData = {
        0, 1, 2,
        1, 3, 2,
    };


    // create a set of colours for the verticies
    vertexColorArray = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
    };
}

Quad::~Quad() {
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

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBufferData.size()*sizeof(GLuint)), indexBufferData.data(), GL_STREAM_DRAW);

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

    dataBound = true;
}

void Quad::SetVertexArray(const std::vector<float> &_vertexArray) {
    vertexArray = _vertexArray;

    if (dataBound) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
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

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind
    glBindVertexArray(0);

    // Disable Attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


//    glBindTexture(GL_TEXTURE_2D, 0);
//    glDisable(GL_TEXTURE_2D);
}

