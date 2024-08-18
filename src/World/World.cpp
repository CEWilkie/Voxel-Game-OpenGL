//
// Created by cew05 on 10/07/2024.
//

#include "glm/gtc/noise.hpp"

#include "World.h"
#include "../Blocks/CreateBlock.h"
#include "CreateBiome.h"
#include "../Window.h"

World::World() {
    // Create skybox
    skybox = CreateBlock({BLOCKID::AIR, 1});

    GLint uLocation;
    uLocation = glGetUniformLocation(window.GetShader(), "worldAmbients.lightingStrength");
    if (uLocation < 0) printf("location not found [worldAmbients.lightingStrength]\n");
    else {
        glUniform1f(uLocation, 1.0f);
    }

    glEnable(GL_DEPTH_TEST);
}

World::~World() = default;

void World::Display() {
    // First draw in the skybox and decorations
    skybox->Display(&skyboxTransformation);

    glEnable(GL_CULL_FACE);
    // Draw solid objects
    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            worldChunks[chunkX][chunkZ]->DisplaySolid();
        }
    }
    glDisable(GL_CULL_FACE);

    // Draw transparent objects
    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            worldChunks[chunkX][chunkZ]->DisplayTransparent();
        }
    }
}

void World::CheckCulling(const Camera &_camera) {
    displayingChunks = 0;

    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {;
            worldChunks[chunkX][chunkZ]->CheckCulling(_camera);
            if (worldChunks[chunkX][chunkZ]->ChunkVisible())
                displayingChunks += 1;
        }
    }

//    printf("DISPLAYING %d / %d CHUNKS\n", displayingChunks, nChunks);
}



/*
 * SKYBOX
 */

void World::SetSkyboxProperties(const Player& player) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = player.GetUsingCamera()->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second*2, 2.0);

    // Set skybox scale
    skyboxTransformation.SetScale({float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3))});

    // Set skybox position centred on the player
    SetSkyboxPosition(player.GetPosition());
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
    float height;

    /*
     * PRIMARY TERRAIN LEVELS
     */

    // Seabed / ContinentBed Generation
    float continentiality = glm::simplex(glm::vec2( _blockPos.x / 2000.0, _blockPos.y / 2000.0));
    continentiality = 1;

    // Primary Noise based around waterlevel
    float baseHeight = glm::simplex(glm::vec2( _blockPos.x / 128.0, _blockPos.y / 128.0));
    baseHeight *= 5;

    // Set the base height of the block
    height = baseHeight * continentiality + MINBLOCKHEIGHT;

    // Secondary base level noise applied
    float secondHeight = glm::simplex(glm::vec2( _blockPos.x / 16.0, _blockPos.y / 16.0));
    secondHeight *= 1;
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

    float mountainFreq = 20; // increase to reduce number of mountains
    height += peakHeight * std::pow(areaHeight, mountainFreq);


    return std::round(height);
}

float World::GenerateBlockHeat(glm::vec3 _blockPos) {
    float heat = glm::simplex(glm::vec2(_blockPos.x / 64.0, _blockPos.z / 64.0));
    heat = (heat + 1) / 2;
    heat *= 20;
    heat += BASETEMP;

    // Relate heat to height (higher = colder)
    heat -= (_blockPos.y / MAXBLOCKHEIGHT) * 10;

    return heat;
}

float World::GenerateBlockVegetation(glm::vec3 _blockPos, float _heat) {
    // Density of vegetation
    float vegetationDensity = glm::simplex(glm::vec2( _blockPos.x / 8.0, _blockPos.z / 8.0));
    vegetationDensity = (vegetationDensity + 1) / 2;

    return vegetationDensity;
}

// Generate the height and temp maps for the given chunk starting pos
ChunkData World::GenerateChunkData(glm::vec2 _chunkPosition) {
    int chunkX = (int)_chunkPosition.x * chunkSize;
    int chunkZ = (int)_chunkPosition.y * chunkSize;
    ChunkData chunkData {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int bx = chunkX + x, bz = chunkZ + z;

            // Get the toplevel (highest y) of the given x z position
            float height = GenerateBlockHeight({bx, bz});
            chunkData.heightMap[x + z * chunkSize] = height;

            // Get the heat value for the block
            float heat = GenerateBlockHeat({bx,height,bz});
            chunkData.heatMap[x + z * chunkSize] = heat;

            // Create block vegetation value (relate to heat, height)
            float vegetation = GenerateBlockVegetation({bx, height, bz}, heat);
            chunkData.plantMap[x + z * chunkSize] = vegetation;
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
    // Ensure that a chunk object is created for the whole world
    for (int x = 0; x < worldSize; x++) {
        for (int z = 0; z < worldSize; z++) {
            float chunkX = (float) x - worldSize / 2.0f;
            float chunkZ = (float) z - worldSize / 2.0f;

            // Get Chunk ChunkData
            ChunkData chunkData = GenerateChunkData({chunkX, chunkZ});
            chunkData.biome = GenerateBiome(GetBiomeIDFromData(chunkData));

            glm::vec3 chunkPos{chunkX, 0, chunkZ};
            worldChunks[x][z] = std::make_unique<Chunk>(chunkPos, chunkData);
            nChunks++;
        }
    }

    // Assign neighbouring chunks to created chunks and invoke terrain and mesh generation
    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            // FRONT, FRONTLEFT, LEFT, BACKLEFT, BACK, BACKRIGHT, RIGHT, FRONTRIGHT
            std::array<Chunk *, 8> adjacentChunks{nullptr};

            for (int dir = 2; dir < numDirections; dir++) {
                adjacentChunks[dir-2] = GetChunkAtPosition(glm::vec3{chunkX, 0, chunkZ} + allDirections[dir]);
            }

            auto st = SDL_GetTicks64();

            worldChunks[chunkX][chunkZ]->SetAdjacentChunks(adjacentChunks);
            worldChunks[chunkX][chunkZ]->GenerateChunk();

            auto et = SDL_GetTicks64();

            chunkSumTicksTaken += et - st;
            nChunksCreated++;
            chunkAvgTicksTaken = chunkSumTicksTaken / nChunksCreated;

        }
        printf("%d Chunk / %d Made. . . \n", nChunksCreated, worldArea);
    }

    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            auto st = SDL_GetTicks64();

            worldChunks[chunkX][chunkZ]->CreateChunkMeshes();

            auto et = SDL_GetTicks64();

            meshSumTicksTaken += et - st;
            nMeshesCreated++;
            meshAvgTicksTaken = meshSumTicksTaken / nMeshesCreated;

        }
    }
}

Chunk* World::GetChunkAtPosition(glm::vec3 _position) const {
    if (_position.x < 0 || _position.x >= worldSize) return nullptr;
    if (_position.z < 0 || _position.z >= worldSize) return nullptr;

    return worldChunks[(int)_position.x][(int)_position.z].get();
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