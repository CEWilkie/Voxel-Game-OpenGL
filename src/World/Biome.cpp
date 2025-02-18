//
// Created by cew05 on 29/07/2024.
//

#include "Biome.h"

#include "glm/gtc/noise.hpp"

Biome::Biome() = default;

Biome::~Biome() = default;

BlockType Biome::GetBlockType(float _hmTopLevel, float _blockY) {

    /*
     * HEIGHT-BASED TERRAIN GENERATION
     */

    // BlockType
    BlockType newBlockType = {STONE, 0};

    // SubSurface Terrain
    if (_blockY == 0) newBlockType = {UNBREAKABLEBLOCK, 0};
    else if (_blockY <= DENSITYLAYER || (_blockY < DENSITYLAYER + 5)) newBlockType = {TOUGHSTONE, 0};
    else if (_blockY < _hmTopLevel - 4) newBlockType = {STONE, 0};

    // Topsurface Terrain
    else if (_blockY <= _hmTopLevel && _blockY < WATERLEVEL + 2) newBlockType = {SAND, 0};
    else if (_blockY < _hmTopLevel && _hmTopLevel < WATERLEVEL + 120) newBlockType = {DIRT, 0};
    else if (_blockY == _hmTopLevel && _hmTopLevel < WATERLEVEL + 120) newBlockType = {GRASS, 0};
    else if (_blockY > _hmTopLevel && _blockY <= WATERLEVEL) newBlockType = {WATER, 0};

    return newBlockType;
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