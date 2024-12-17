//
// Created by cew05 on 29/07/2024.
//

#include "Biome.h"

#include "glm/gtc/noise.hpp"

Biome::Biome() = default;

Biome::~Biome() = default;

BlockType Biome::GetBlockType(float _hmTopLevel, float _blockDensity, float _blockY) {
    // Returns the type of block that generates at the given Y value in the biome
    BlockType newBlockData;

    // Density was not calculated
    if (_blockDensity == -2) {
        if (_blockY < WATERLEVEL && _blockY > _hmTopLevel) newBlockData = {WATER, 0};
        else if (_blockY <= _hmTopLevel && _blockY > _hmTopLevel - 4 && _hmTopLevel < WATERLEVEL + 2)  newBlockData = {SAND, 0};
        else if (_blockY > _hmTopLevel) newBlockData = {AIR, 0};
        else if (_blockY == _hmTopLevel && _blockY >= WATERLEVEL + 120) newBlockData = {BLOCKID::STONE, 0};
        else if (_blockY == _hmTopLevel) newBlockData = {BLOCKID::GRASS, 0};
        else if (_blockY > _hmTopLevel - 4 && _hmTopLevel < WATERLEVEL + 120) newBlockData = {BLOCKID::DIRT, 0};
        else if (_blockY > 0) newBlockData = {BLOCKID::STONE, 0};
        else newBlockData = {BLOCKID::UNBREAKABLEBLOCK, 0};
    }

    // Density used
    else {
        if (_blockY < WATERLEVEL && _blockDensity < 0.5) newBlockData = {WATER, 0};
        else if (_blockDensity < 0.4) newBlockData = {AIR, 0};
        else if (_blockDensity < 0.6) newBlockData = {DIRT, 0};
        else newBlockData = {STONE, 0};
    }

    return newBlockData;
}

float Biome::GetAttribute(BIOMEATTRIB _attribute) const {
    switch (_attribute) {
        case BIOMEATTRIB::HEIGHT:
            return minHeight;

        case BIOMEATTRIB::TEMP:
            return minTemp;

        default:
            return 0.0f;
    }
}



Hills::Hills() {
    biomeID = HILLS;

    minHeight = WATERLEVEL + 10;

}

Marshlands::Marshlands() {
    biomeID = SWAMP;

    minHeight = WATERLEVEL + 1;

}


Mountains::Mountains() {
    biomeID = MOUNTAINS;

    minHeight = WATERLEVEL + 30;

}

Plains::Plains() {
    biomeID = PLAINS;

    minHeight = WATERLEVEL + 3;

}

Beach::Beach() {
    biomeID = BEACH;

    minHeight = WATERLEVEL;

}

OceanShores::OceanShores() {
    biomeID = OCEAN;

    minHeight = WATERLEVEL - 3;

}