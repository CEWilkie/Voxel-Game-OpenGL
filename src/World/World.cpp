//
// Created by cew05 on 10/07/2024.
//

#include "glm/gtc/noise.hpp"

#include "World.h"
#include "../Blocks/CreateBlock.h"
#include "CreateBiome.h"

World::World() {
    // Create skybox
    skybox = CreateBlock({BLOCKID::AIR, 1});

    glEnable(GL_DEPTH_TEST);
}

World::~World() = default;

void World::Display() {
    // First draw in the skybox and decorations
    skybox->Display(&skyboxTransformation);

    glEnable(GL_CULL_FACE);
    // Draw solid objects
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                worldChunks[chunkX][chunkY][chunkZ]->DisplaySolid();
            }

    // Draw transparent objects
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                worldChunks[chunkX][chunkY][chunkZ]->DisplayTransparent();
            }

    glDisable(GL_CULL_FACE);

}

void World::CheckCulling(const Camera &_camera) {
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                worldChunks[chunkX][chunkY][chunkZ]->CheckCulling(_camera);
            }
}



/*
 * SKYBOX
 */

void World::SetSkyboxProperties(const Camera *camera) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = camera->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second*2, 2.0);

    // Set skybox scale
    skyboxTransformation.SetScale({float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3))});

    // Set skybox position centred on the player
    SetSkyboxPosition(camera->GetPosition());
}

void World::SetSkyboxPosition(glm::vec3 _position) {
    glm::vec3 originFromCentre{_position - (skyboxTransformation.GetLocalScale() / 2.0f)};
    originFromCentre.y += skyboxTransformation.GetLocalScale().y;
    skyboxTransformation.SetPosition(originFromCentre);

    skyboxTransformation.UpdateModelMatrix();
}


/*
 * WORLD GENERATION
 */

void World::GenerateWorld() {
    // Generate the world terrain, and map biomes to the chunks
    GenerateTerrain();


    printf("AVG CHUNK CREATION: %llu TICKS TAKEN\n", chunkAvgTicksTaken);
    printf("AVG MESH CREATION: %llu TICKS TAKEN\n", meshAvgTicksTaken);

    printf("SUM CHUNK CREATION: %llu TICKS TAKEN\n", chunkSumTicksTaken);
    printf("SUM MESH CREATION: %llu TICKS TAKEN\n", meshSumTicksTaken);
}


float World::GenerateBlockHeight(glm::vec2 _blockPos) {
    float height = 0;

    /*
     * PRIMARY TERRAIN LEVELS
     */

    // Primary Noise based around waterlevel
    float baseHeight = glm::simplex(glm::vec2( _blockPos.x / 128.0, _blockPos.y / 128.0));
    baseHeight *= 5;

    baseHeight += WATERLEVEL;  // Ensure minimum generation value for solid blocks
    height = baseHeight;

    // Secondary base level noise applied to provide more jaggedness
    float secondHeight = glm::simplex(glm::vec2( _blockPos.x / 32.0, _blockPos.y / 32.0));
    secondHeight *= 2;
    height += secondHeight;

    /*
     *  MOUNTAIN GENERATION
     */

    // Produce noise values for mountain
    float peakHeight = glm::simplex(glm::vec2( _blockPos.x / 128.0, _blockPos.y / 128.0));
    peakHeight = (peakHeight + 1) / 2;
    peakHeight *= 128.0;

    // Determine if mountain should generate
    float areaHeight = glm::simplex(glm::vec2( _blockPos.x / 500.0, _blockPos.y / 500.0));
    areaHeight = (areaHeight + 1) / 2;

    float mountainFreq = 10; // increase to reduce number of mountains
    height += peakHeight * std::pow(areaHeight, mountainFreq);


    return std::round(height);
}

// Generate the height and temp maps for the given chunk starting pos
ChunkData World::GenerateChunkData(glm::vec2 _chunkPosition) {
    int chunkX = (int)_chunkPosition.x * chunkSize;
    int chunkZ = (int)_chunkPosition.y * chunkSize;
    ChunkData chunkData {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            chunkData.heightMap[x + z * chunkSize] = GenerateBlockHeight({chunkX+x, chunkZ+z});

            // get positive heat value between 0 and max heat
            float heat = glm::simplex(glm::vec2(x / 64.0, z / 64.0));
            heat = (heat + 1) / 2;
            heat *= 20;

            // Relate heat to height (higher = colder)
            heat -= (chunkData.heightMap[x + z * chunkSize]/MAXCHUNKHEIGHT) * 10;

            chunkData.heatMap[x + z * chunkSize] = heat;
        }
    }

    return chunkData;
}



/*
 * Ensure that a biome of type BIOMEID has been generated for the world
 */

Biome* World::GenerateBiome(BIOMEID _biomeID) {
    // If the biome has been generated before then exit
    for (auto& uniqueBiome : uniqueBiomes) {
        if (uniqueBiome->GetBiomeID() == _biomeID) return uniqueBiome.get();
    }

    // Else required to create a new unique biome
    uniqueBiomes.emplace_back(CreateBiome(_biomeID));
    return uniqueBiomes.back().get();
}


void World::GenerateTerrain() {
    // Ensure that a chunk is created for the whole world
    for (int x = 0; x < worldSize; x++) {
        for (int z = 0; z < worldSize; z++) {
            float chunkX = (float)x - worldSize / 2.0f;
            float chunkZ = (float)z - worldSize / 2.0f;

            // Get Chunk ChunkData
            ChunkData chunkData = GenerateChunkData({chunkX, chunkZ});
            chunkData.biome = GenerateBiome(GetBiomeIDFromData(chunkData));

            for (int y = 0; y < worldHeight; y++) {
                glm::vec3 chunkPos{chunkX, y, chunkZ};

                worldChunks[x][y][z] = std::make_unique<Chunk>(chunkPos, chunkData);
                worldChunks[x][y][z]->GenerateChunk();
            }
        }
    }

    // Assign neighbouring chunks to created chunks and invoke terrain generation for non-edge chunks
    for (int chunkX = 1; chunkX < worldSize-1; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 1; chunkZ < worldSize-1; chunkZ++) {
                // TOP, BOTTOM, FRONT, FRONTLEFT, LEFT, BACKLEFT, BACK, BACKRIGHT, RIGHT, FRONTRIGHT
                std::array<Chunk*, numDirections> adjacentChunks {nullptr};

                for (int dir = 0; dir < numDirections; dir++) {
                    adjacentChunks[dir] = GetChunkAtPosition(glm::vec3{chunkX, chunkY, chunkZ} + allDirections[dir]);
                }

                worldChunks[chunkX][chunkY][chunkZ]->SetAdjacentChunks(adjacentChunks);
                worldChunks[chunkX][chunkY][chunkZ]->CreateBlockMeshes();
            }
}

Chunk* World::GetChunkAtPosition(glm::vec3 _position) const {
    if (_position.x < 0 || _position.x >= worldSize) return nullptr;
    if (_position.y < 0 || _position.y >= worldHeight) return nullptr;
    if (_position.z < 0 || _position.z >= worldSize) return nullptr;

    return worldChunks[(int)_position.x][(int)_position.y][(int)_position.z].get();
}

Biome* World::GetBiome(BIOMEID _biomeID) {
    // Fetch biome
    for (auto& uniqueBiome : uniqueBiomes) {
        if (uniqueBiome->GetBiomeID() == _biomeID) return uniqueBiome.get();
    }

    // Biome did not exist?
    printf("WARNING : BIOME NOT CREATED BEFORE USAGE REQUEST\n");
    uniqueBiomes.emplace_back(CreateBiome(_biomeID));
    return uniqueBiomes.back().get();
}