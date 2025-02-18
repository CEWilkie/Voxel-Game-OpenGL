//
// Created by cew05 on 18/12/2024.
//

#ifndef VOXELGAME_3DSIMPLEXBLOCKDENSITY_H
#define VOXELGAME_3DSIMPLEXBLOCKDENSITY_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "../WorldGenConsts.h"

inline float BlockDensity(const glm::vec3& _pos, float _scale, int _octaves, float _persistence, float _lacunarity) {
    float amplitude = 1, frequency = 1, result = 0.0f;
    worldGenerationRandom.seed(worldSeed);

    std::vector<glm::vec2> octaveOffsets;
    for (int o = 0; o < _octaves; ++o) {
        int randX = (int)worldGenerationRandom();
        int randZ = (int)worldGenerationRandom();
        octaveOffsets.emplace_back(randX/10'000, randZ/10'000);
    }

    // ensure scale and octaves is minimum of > 0
    if (_scale <= 0) _scale = 0.001f;
    if (_octaves <= 0) _octaves = 1;

    // for each octave
    for (int oct = 0; oct < _octaves; ++oct) {
        float posX = ((_pos.x + octaveOffsets[oct].x) / _scale) * frequency;
        float posY = (_pos.y / _scale) * frequency;
        float posZ = ((_pos.z + octaveOffsets[oct].y) / _scale) * frequency;

        // retrieve basic simplex value for octave between -1 -> +1
        float simplexVal = glm::simplex(glm::vec3{posX, posY, posZ});
        result += simplexVal * amplitude;

        // Update amplitude and Freq for next octave
        amplitude *= _persistence;
        frequency *= _lacunarity;

        // If the density value cannot be flipped to be > minDensity, break early
        float maxChange;
        for (int remOcts = oct; remOcts < _octaves - 1; ++remOcts) {
            maxChange += amplitude; // assuming simplex produces abs max of 1
            amplitude *= _persistence;
        }

        // If the sign could change continue, else break
        if (abs(maxChange) + abs(result) > abs(result + maxChange) ||
        abs(maxChange) + abs(result) > abs(result - maxChange)) {
            continue;
        }
        else break;
    }

    // Return result to -1 -> +1 range from minMaxDensity
//    return ((result - minResult) / (maxResult - minResult)) * 2 - 1;
    return result;
}


inline float CaveDensity(const glm::vec3& _pos, float _scale, int _octaves, float _persistence, float _lacunarity) {
    float amplitude = 1, frequency = 1, result = 0.0f;
    worldGenerationRandom.seed(worldSeed);

    std::vector<glm::vec3> octaveOffsets;
    for (int o = 0; o < _octaves; ++o) {
        int randX = (int)worldGenerationRandom();
//        int randY = (int)worldGenerationRandom();
        int randZ = (int)worldGenerationRandom();
        octaveOffsets.emplace_back(randX/10'000, 0, randZ/10'000);
    }

    // ensure scale is minimum of > 0
    if (_scale <= 0) _scale = 0.001f;

    // for each octave
    for (int oct = 0; oct < _octaves; ++oct) {
        float posX = (_pos.x / _scale) * frequency + octaveOffsets[oct].x;
        float posY = (_pos.y / _scale) * frequency + octaveOffsets[oct].y;
        float posZ = (_pos.z / _scale) * frequency + octaveOffsets[oct].z;

        // retrieve basic simplex value for octave between -1 -> +1
        float simplexVal = glm::simplex(glm::vec3{posX, posY, posZ});
        result += simplexVal * amplitude;

        // Update amplitude and Freq for next octave
        amplitude *= _persistence;
        frequency *= _lacunarity;
    }

    return result;
}

#endif //VOXELGAME_3DSIMPLEXBLOCKDENSITY_H
