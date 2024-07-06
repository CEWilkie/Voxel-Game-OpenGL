//
// Created by cew05 on 16/04/2024.
//

#include "Triangle.h"

Triangle::Triangle() {
    // Create RGB surface from image
    SDL_Surface* img;
    img = IMG_Load("../resources/coin64x.png");
    if (img == nullptr) {
        LogError("Failed to load img", SDL_GetError(), false);
        return;
    }

    // Bind texture
    glGenTextures(1, &textureObject);
    glBindTexture(GL_TEXTURE_2D, textureObject);
    printf("TO : %u\n", textureObject);

    // specify texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

    // texture stretch properties
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Free SDL surface and unbind texture
    SDL_FreeSurface(img);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Triangle::~Triangle() {
    // Deallocate texture
    printf("called");
    glDeleteTextures(1, &textureObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void Triangle::ConstructTriangle(GLint _numAttribs, const std::vector<float>& _vertexArray) {
    // Set verticies
    vertexArray = _vertexArray;
    numAttribs = _numAttribs;

    // bind object id
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    printf("VAO : %u\n", vertexArrayObject);

    // bind vertexArray array
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexArray.size() * sizeof(float)), vertexArray.data(), GL_DYNAMIC_DRAW);
    printf("VBO : %u\n", vertexBufferObject);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, numAttribs, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Unbind arrays / buffers
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Triangle::Display() const {
    // Bind
    glBindTexture(GL_TEXTURE_2D, textureObject);
    glEnable(GL_TEXTURE_2D);
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    // Draw
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vertexArray[0], vertexArray[1], vertexArray[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vertexArray[3], vertexArray[4], vertexArray[5]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vertexArray[6], vertexArray[7], vertexArray[8]);
    glEnd();

    //glDrawArrays(GL_TRIANGLES, 0, 3);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

