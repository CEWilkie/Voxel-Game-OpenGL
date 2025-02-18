//
// Created by cew05 on 10/11/2024.
//

#ifndef VOXELGAME_2DSIMPLEXNOISE_H
#define VOXELGAME_2DSIMPLEXNOISE_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "../WorldGenConsts.h"

/*
 * Based upon Sebastian Lague's noise function in
 * https://www.youtube.com/watch?v=MRNFcywkUSA&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3&index=3
 */

/*
 * Generate Complex and seeded Noise value from usage of glm::simplex
 */

inline float ComplexNoise(const glm::vec2& _pos, float _scale, int _octaves, float _persistence, float _lacunarity) {
    float amplitude = 1, frequency = 1, result = 0.0f;
    worldGenerationRandom.seed(worldSeed);

    std::vector<glm::vec2> octaveOffsets;
    for (int o = 0; o < _octaves; ++o) {
        int randX = (int)worldGenerationRandom();
        int randZ = (int)worldGenerationRandom();
        octaveOffsets.emplace_back(randX/10'000, randZ/10'000);
    }

    // ensure scale is minimum of > 0
    if (_scale <= 0) _scale = 0.001f;

    // for each octave
    for (int oct = 0; oct < _octaves; ++oct) {
        float posX = ((_pos.x + octaveOffsets[oct].x) / _scale) * frequency;
        float posZ = ((_pos.y + octaveOffsets[oct].y) / _scale) * frequency;


        // retrieve basic simplex value for octave between -1 -> +1
        float simplexVal = glm::simplex(glm::vec2{posX, posZ});
        result += simplexVal * amplitude;

        amplitude *= _persistence;
        frequency *= _lacunarity;
    }

    // return to -1 -> +1 scale
//    return ((result - min) / (max - min)) * 2 - 1;
    return result;
}


inline float ComplexNoiseLimited(const glm::vec2& _pos, float _scale, int _octaves, float _persistence, float _lacunarity, float _minLimit, float _maxLimit) {
    float result = ComplexNoise(_pos, _scale, _octaves, _persistence, _lacunarity);

    float min = 0, amplitude = 1;
    for (int oct = 0; oct < _octaves; ++oct) {
        min -= amplitude;
        amplitude *= _persistence;
    }
    float max = -min;

    return ((result - min) / (max - min)) * (_maxLimit - _minLimit) + _minLimit;
}


#endif //VOXELGAME_2DSIMPLEXNOISE_H
