
//
// Created by cew05 on 23/07/2024.
//

#include "NaturalBlocks.h"
#include "../Textures/TextureManager.h"

/*
 * STONE
 */

Stone::Stone(glm::vec3 _position) : Cube() {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    textureSheetID = TEXTURESHEET::NATURAL;
    textureOrigin = {1,1};
    UpdateTextureData();
}

Dirt::Dirt(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    textureSheetID = TEXTURESHEET::NATURAL;
    textureOrigin = {4,2};
    UpdateTextureData();
}

Grass::Grass(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    textureSheetID = TEXTURESHEET::NATURAL;
    textureOrigin = {7,1};
    UpdateTextureData();
}

Water::Water(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    textureSheetID = TEXTURESHEET::NATURAL;
    textureOrigin = {10,2};
    UpdateTextureData();
}

Air::Air(glm::vec3 _position) {
    // set the position of the cube
    SetPositionOrigin(_position);
    UpdateModelMatrix();

    // Set the texture and texture origin
    textureSheetID = TEXTURESHEET::NATURAL;
    textureOrigin = {12,1};
    UpdateTextureData();
}