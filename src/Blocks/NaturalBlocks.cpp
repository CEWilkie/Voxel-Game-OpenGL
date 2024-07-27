
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
    sheet = TEXTURESHEET::NATURAL;
    origin = {1,1};
}

Dirt::Dirt(int _variant) {
    blockData = {BLOCKID::DIRT, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::NATURAL;
    origin = {4,2};
}

Grass::Grass(int _variant) {
    blockData = {BLOCKID::GRASS, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::NATURAL;
    origin = {7,1};
}

Water::Water(int _variant) {
    blockData = {BLOCKID::WATER, _variant};

    // Set the texture and texture origin
    sheet = TEXTURESHEET::NATURAL;
    origin = {10,2};

}

Air::Air(int _variant) {
    blockData = {BLOCKID::AIR, _variant};

}