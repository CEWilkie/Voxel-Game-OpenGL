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

            // Get main biome noise value between 0 and noiseMultiplier
            float biomeNoise = glm::simplex(glm::vec2(cubeX / scaleX, cubeZ / scaleY));
            biomeNoise = (biomeNoise + 1) / 2;
            biomeNoise *= noiseMultiplier;

            // Apply to height map
            heightMap[x + z*chunkSize] = (int)biomeNoise + minHeight;

            // secondary noise value
            float secondaryNoise = glm::simplex(glm::vec2(cubeX / 16, cubeZ / 16));
            secondaryNoise = (secondaryNoise + 1) / 2;
            secondaryNoise *= 2;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)secondaryNoise;
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

    noiseMultiplier = 64.0f;
    minHeight = 45;
    scaleX = 128.0f;
    scaleY = 128.0f;
}