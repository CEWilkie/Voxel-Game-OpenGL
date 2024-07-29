//
// Created by cew05 on 29/07/2024.
//

#include "Biome.h"

#include "glm/gtc/noise.hpp"

Biome::Biome() = default;

Biome::~Biome() = default;


std::array<int, chunkArea> Biome::ChunkHeightMap(glm::vec3 _chunkOrigin) const {
    std::array<int, chunkArea> heightMap {};
    // Take a flat xz plane of the chunk, and determine height values for each xz pillar
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int cubeX = x + (int)_chunkOrigin.x;
            int cubeZ = z + (int)_chunkOrigin.z;

            // Get positive noise value between 0 and noiseMultiplier
            float simplexNoise = glm::simplex(glm::vec2(cubeX / scaleX, cubeZ / scaleY));
            simplexNoise = (simplexNoise + 1) / 2;
            simplexNoise *= noiseMultiplier;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)simplexNoise + minHeight;
        }
    }

    return heightMap;
}



BlockType Biome::GetBlockType(float _hmTopLevel, float _blockY) {
    // Returns the type of block that generates at the given Y value in the biome
    BlockType newBlockData;

    // Determine block type
    if (_blockY < WATERLEVEL && _blockY > _hmTopLevel) newBlockData = {WATER, 0};
    else if (_blockY > _hmTopLevel) newBlockData = {AIR, 0};
    else if (_blockY == _hmTopLevel) newBlockData = {BLOCKID::GRASS, 0};
    else if (_blockY > _hmTopLevel - 4) newBlockData = {BLOCKID::DIRT, 0};
    else newBlockData = {BLOCKID::STONE, 0};

    return newBlockData;
}





Marshlands::Marshlands() {
    biomeID = MARSHLANDS;

    noiseMultiplier = 4.0f;
    minHeight = 32;
    scaleX = 16.0f;
    scaleY = 16.0f;
}


Mountains::Mountains() {
    biomeID = MOUNTAINS;

    noiseMultiplier = 16.0f;
    minHeight = 45;
    scaleX = 64.0f;
    scaleY = 64.0f;
}