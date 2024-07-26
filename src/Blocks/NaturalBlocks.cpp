
//
// Created by cew05 on 23/07/2024.
//

#include "NaturalBlocks.h"
#include "../Textures/TextureManager.h"

/*
 * STONE
 */

Stone::Stone(int _variant) {
    SetBlockData({BLOCKID::STONE, _variant});

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {1,1});
}

Dirt::Dirt(int _variant) {
    SetBlockData({BLOCKID::DIRT, _variant});

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {4,2});
}

Grass::Grass(int _variant) {
    SetBlockData({BLOCKID::GRASS, _variant});

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {7,1});
}

Water::Water(int _variant) {
    SetBlockData({BLOCKID::WATER, _variant});

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::NATURAL, {10,2});

    // Set transparency
    transparent = true;
}

Air::Air(int _variant) {
    SetBlockData({BLOCKID::AIR, _variant});

    // Set the texture and texture origin
    SetTexture(TEXTURESHEET::WORLD, {12,1});

    // Set transparency
    transparent = true;
}