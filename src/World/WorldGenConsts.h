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
static const int MINCHUNKHEIGHT = 25;
static const int MAXCHUNKHEIGHT = 128;

// SIZING OF THE INITIAL WORLD AREA GENERATED
static const int worldSize = 8;
static const int worldHeight = 16; // from 0 to n chunks high (n x 16 blocks)
static const int worldArea = worldSize * worldSize;
static const int worldVolume = worldArea * worldHeight;

static const double BIOMESCALEX = 64.0f;
static const double BIOMESCALEZ = 64.0;


/*
 * CHUNK VALUES
 */

// SIZE OF THE CHUNKS
static const int chunkSize = 16; // must be power of 2 for subchunk division ie 2, 4, 8, 16 | 32, 64, 128, ... ( too big)
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkSize;

// TRACKING TIME FOR CREATING CHUNKS
inline int nChunksCreated;
inline Uint64 chunkAvgTicksTaken = 0;
inline Uint64 chunkSumTicksTaken = 0;

// TRACKING TIME FOR CREATING BLOCK MESHES
inline int nMeshesCreated;
inline Uint64 meshAvgTicksTaken = 0;
inline Uint64 meshSumTicksTaken = 0;

// DIRECTIONS FOR CHECKING ADJACENT CHUNKS / BLOCKS / BIOMES
inline glm::vec3 dirTop{0, 1, 0};
inline glm::vec3 dirBottom{0, -1, 0};
inline glm::vec3 dirFront{-1, 0, 0};
inline glm::vec3 dirFrontLeft{-1, 0, -1};
inline glm::vec3 dirLeft{0, 0, -1};
inline glm::vec3 dirBackLeft{1, 0, -1};
inline glm::vec3 dirBack{1, 0, 0};
inline glm::vec3 dirBackRight{1, 0, 1};
inline glm::vec3 dirRight{0, 0, 1};
inline glm::vec3 dirFrontRight{-1, 0, 1};

// list of all directions
const int numDirections = 10;
inline std::array<glm::vec3, numDirections> allDirections { dirTop, dirBottom, dirFront, dirFrontLeft, dirLeft, dirBackLeft,
                                                 dirBack, dirBackRight, dirRight, dirFrontRight};

// HEIGHTMAP STRUCT
struct HeightMap {
    std::array<int, chunkArea> heightMap {};
    bool isEmpty = true;
};

typedef std::array<std::array<std::array<BlockType, chunkSize>, chunkSize>, chunkSize> chunkTerrainArray;
typedef std::array<std::array<std::array<float, chunkSize>, chunkSize>, chunkSize> chunkDensityArray;
typedef std::array<float, chunkArea> ChunkDataMap;



// MOVEMENT VALUES

const inline float JUMPHEIGHT = 1.2f;
const inline float JUMPDURATION = 0.2f;
const inline float GRAVITY = -(JUMPHEIGHT / (2 * std::pow(JUMPDURATION, 2.0f)));
const inline float JUMPSPEED = sqrtf(2 * JUMPHEIGHT * -GRAVITY);



#endif //UNTITLED7_WORLDGENCONSTS_H
