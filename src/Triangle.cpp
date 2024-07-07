//
// Created by cew05 on 16/04/2024.
//

#include "Triangle.h"

#include <cmath>
#include <random>
#include <algorithm>

Triangle::Triangle() {
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

    // Create random vertex positions at set distance from centre
    // x = rsin(theta), y = rcos(theta)   [radians]
    std::vector<double> usedThetas {};
    for (int i = 0; i < 3; i++) {
        // Fetch angle
        double theta;
        while (true) {
            theta = ((int(mt()) % 200) / 100.0) * M_PI;
            if (!std::any_of(usedThetas.begin(), usedThetas.end(), [&](double prevTheta){
                return (theta < (prevTheta + 0.25*M_PI)) && (theta > (prevTheta - 0.25*M_PI));
            })) {
                usedThetas.push_back(theta);
                break;
            }
        }
        auto x = r * (float)sin(theta);
        auto y = r * (float)cos(theta);
        auto z = float((int(mt()) % 100) / 100.0);

        printf("V: %f %f %f\n", cx+x, cy+y, z);

        vertexArray.push_back(cx + x);
        vertexArray.push_back(cy + y);
        vertexArray.push_back(z);
    }

    // Now create a set of colours for the verticies
    vertexColorArray = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };

//    // Create RGB surface from image
//    SDL_Surface* surface;
//    surface = IMG_Load("../resources/coin64x.png");
//    if (surface == nullptr) {
//        LogError("Failed to load surface", SDL_GetError(), false);
//        return;
//    }
//
//    // Determine format of surface data
//    GLint data_fmt;
//    Uint8 test = SDL_MapRGB(surface->format, 0xAA, 0xBB, 0xCC) & 0xFF;
//    if      (test==0xAA) data_fmt = GL_RGB;
//    else if (test==0xCC) data_fmt = GL_BGR; //GL_BGR;
//    else {
//        printf("Error: \"Loaded surface was neither RGB or BGR!\""); return;
//    }
//
//    // Bind texture
//    glGenTextures(1, &textureObject);
//    glBindTexture(GL_TEXTURE_2D, textureObject);
//    printf("TO : %u\n", textureObject);
//
//    // specify texture data
//    glTexImage2D(GL_TEXTURE_2D, 0, data_fmt, surface->w, surface->h, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
//
//    // texture stretch properties
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//
//    // Free SDL surface and unbind texture
//    SDL_FreeSurface(surface);
//    glBindTexture(GL_TEXTURE_2D, 0);
}

Triangle::~Triangle() {
    // Deallocate texture
    glDeleteTextures(1, &textureObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &colorBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void Triangle::ConstructTriangle() {
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


void Triangle::Move() {
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


void Triangle::Display() const {
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

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Unbind
    glBindVertexArray(0);

    // Disable Attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


//    glBindTexture(GL_TEXTURE_2D, 0);
//    glDisable(GL_TEXTURE_2D);
}

