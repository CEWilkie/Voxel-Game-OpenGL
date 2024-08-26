//
// Created by cew05 on 30/07/2024.
//

#ifndef UNTITLED7_WORLDGENCONSTS_H
#define UNTITLED7_WORLDGENCONSTS_H

#include <SDL.h>

/*
 * WORLD VALUES
 */

// UNIVERSAL CONSTANTS FOR GENERATION
static const int WATERLEVEL = 60;
static const int MINBLOCKHEIGHT = WATERLEVEL;
static const int MAXBLOCKHEIGHT = 128;

static const int MINTEMP = -20;
static const int BASETEMP = 15;
static const int MAXTEMP = 40;


// MAX SIZE OF THE WORLD AREA TO BE LOADED
static const int loadRadius = 8;
static const int worldSize = (1 + loadRadius*2) + 2; // + 2 for border chunks to permit structure generation at world chunk borders
static const int worldArea = worldSize * worldSize;

static long long int worldSeed = time(nullptr);


/*
 * CHUNK VALUES
 */

// SIZE OF THE CHUNKS
static const int chunkSize = 16;
static const int chunkHeight = 256;
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkHeight;

// TRACKING TIME FOR CREATING CHUNKS
inline int nChunksCreated;
inline Uint64 chunkAvgTicksTaken = 0;
inline Uint64 chunkSumTicksTaken = 0;

// TRACKING TIME FOR CREATING BLOCK MESHES
inline int nMeshesCreated;
inline Uint64 meshAvgTicksTaken = 0;
inline Uint64 meshSumTicksTaken = 0;


// CHUNK TYPEDEFS
namespace ChunkDataTypes {
    struct ChunkBlock {
        BlockType type {AIR, 0};
        BlockAttributes attributes;
    };
    typedef std::array<std::array<std::array<ChunkBlock, chunkSize>, chunkHeight>, chunkSize> TerrainArray;
    typedef std::array<std::array<std::array<float, chunkSize>, chunkHeight>, chunkSize> DensityArray;
    typedef std::array<float, chunkArea> DataMap;
}


/*
 * MOVEMENT VALUES
 */

const inline float JUMPHEIGHT = 1.2f;
const inline float JUMPDURATION = 0.2f;
const inline float GRAVITY = -(JUMPHEIGHT / (2 * std::pow(JUMPDURATION, 2.0f)));
const inline float JUMPSPEED = sqrtf(2 * JUMPHEIGHT * -GRAVITY);




#endif //UNTITLED7_WORLDGENCONSTS_H
