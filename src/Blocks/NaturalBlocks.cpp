
//
// Created by cew05 on 23/07/2024.
//

#include "NaturalBlocks.h"
#include "../Textures/TextureManager.h"

/*
 * STONE
 */

Stone::Stone(glm::vec3 _position) : Block() {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {1,1});
}

Dirt::Dirt(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {4,2});
}

Grass::Grass(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {7,1});
}

Water::Water(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {10,2});

    // Set transparency
    transparent = true;
}

Air::Air(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {12,1});

    // Set transparency
    transparent = true;
}