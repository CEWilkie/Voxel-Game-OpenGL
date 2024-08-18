
//
// Created by cew05 on 23/07/2024.
//

#include "NaturalBlocks.h"
#include "../Textures/TextureManager.h"

/*
 * STONE
 */

Stone::Stone(int _variant) {
    blockData = {BLOCKID::STONE, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::TERRAIN;
    origin = {1,1};
}

Dirt::Dirt(int _variant) {
    blockData = {BLOCKID::DIRT, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::TERRAIN;
    origin = {4,2};
}

Grass::Grass(int _variant) {
    blockData = {BLOCKID::GRASS, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::TERRAIN;
    origin = {7,1};
}

Sand::Sand(int _variant) {
    blockData = {BLOCKID::SAND, _variant};

    // Set texture and texture origin
    sheet = TEXTURESHEET::TERRAIN;
    origin = {13, 1};
}

Water::Water(int _variant) {
    blockData = {BLOCKID::WATER, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::TERRAIN;
    origin = {10,2};

    // Set attributes
    transparent = 1;
    liquid = 1;
    breakable = 0;
    canInteractThroughBlock = 1;
}

Air::Air(int _variant) {
    blockData = {BLOCKID::AIR, _variant};

    sheet = TEXTURESHEET::WORLD;

    // Variants used for the skybox decorations
    if (_variant == 1) { // skybox
        origin = {1,1};
    }

    if (_variant == 2) { // sun
        origin = {4,2};
    }

    if (_variant == 2) { // moon
        origin = {7,1};
    }

    transparent = 1;
    breakable = 0;
    canInteractThroughBlock = 1;
}

Leaves::Leaves(int _variant) {
    blockData = {BLOCKID::LEAVES, _variant};
    sheet = TEXTURESHEET::NATURAL;

    if (_variant == 0) {
        origin = {1,1};
    }

    transparent = 1;
}