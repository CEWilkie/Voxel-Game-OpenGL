//
// Created by cew05 on 07/07/2024.
//

#include "Shape.h"

Shape::Shape() {
    // Generate objectIDs
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &colorBufferObject);

    // Generate object colour
    vertexColorArray = {
            // Triangle
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            // Second set of colours for quad
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };
}


Shape::~Shape() {
    // Delete buffers, arrays
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &colorBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}


void Shape::SetVertexArray(const std::vector<float>& _vertexArray) {
    // Set vertex array
    vertexArray = _vertexArray;

    // bind to object
    glBindVertexArray(vertexArrayObject);

    // Bind vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_STREAM_DRAW);

    // Vertex Position Attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, vertexPositionAttribs, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*vertexPositionAttribs, nullptr);

    // Bind colour buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexColorArray.size() * sizeof(float)), vertexColorArray.data(), GL_STATIC_DRAW);

    // Vertex Colour Attributes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, vertexColorAttribs, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(float)*vertexColorAttribs, nullptr);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Shape::Display() const {
    // bind to object
    glBindVertexArray(vertexArrayObject);

    // Enable Attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, GLint(vertexArray.size()/vertexPositionAttribs));

    // Unbind
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}