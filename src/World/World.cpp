//
// Created by cew05 on 10/07/2024.
//

#include "glm/gtc/noise.hpp"

#include "World.h"
#include "../Blocks/CreateBlock.h"

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
    double maxSqrd = std::pow(minMax.second-1, 2.0);

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
    // Generate Biome Map
    GenerateBiomeMap();

    // First generate the world terrain
    GenerateTerrain();


    printf("AVG CHUNK CREATION: %llu TICKS TAKEN\n", chunkAvgTicksTaken);
    printf("AVG MESH CREATION: %llu TICKS TAKEN\n", meshAvgTicksTaken);

    printf("SUM CHUNK CREATION: %llu TICKS TAKEN\n", chunkSumTicksTaken);
    printf("SUM MESH CREATION: %llu TICKS TAKEN\n", meshSumTicksTaken);
}


void World::GenerateBiomeMap() {
    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            // Get positive noise value between 0 and 2
            float biomeID = glm::simplex(glm::vec2(chunkX / 8.0, chunkZ / 8.0));
            biomeID = (biomeID + 1) / 2;
            biomeID *= 2;

            biomeID = std::round(biomeID);
            // Check if biome is new to the world
            if (!std::any_of(uniqueBiomes.begin(), uniqueBiomes.end(),
            [&](std::pair<std::unique_ptr<Biome>, int> &uniqueBiome) {
                // Biome is different
                if (uniqueBiome.first->GetBiomeID() != (BIOMEID)biomeID) return false;

                // Already exists, increment count
                uniqueBiome.second += 1;
                return true;
            })) {
                // create a new block of the specified type, and create mesh for block
                if (biomeID == HILLS) uniqueBiomes.emplace_back(std::make_unique<Biome>(), 1);
                if (biomeID == MARSHLANDS) uniqueBiomes.emplace_back(std::make_unique<Marshlands>(), 1);
            }

            // Apply to height map
            biomeMap[chunkX + chunkZ*worldSize] = (BIOMEID)biomeID;
        }
    }
}



void World::GenerateTerrain() {
    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
            auto biomeID = (BIOMEID) biomeMap[chunkX + chunkZ * worldSize];
            Biome* biome;
            for (const auto& biomePair : uniqueBiomes) {
                if (biomePair.first->GetBiomeID() == biomeID) biome = biomePair.first.get();
            }

            for (int chunkY = 0; chunkY < worldHeight; chunkY++) {
                glm::vec3 chunkPos{chunkX - worldSize / 2, chunkY, chunkZ - worldSize / 2};

                worldChunks[chunkX][chunkY][chunkZ] = std::make_unique<Chunk>(chunkPos, biome);
            }
        }
    }

    // Assign neighbouring chunks to created chunks
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                // TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT
                std::array<Chunk*, 6> adjacentChunks {nullptr};
                std::vector<glm::vec3> positionOffsets {
                        ChunkDataTypes::adjTop, ChunkDataTypes::adjBottom, ChunkDataTypes::adjFront,
                        ChunkDataTypes::adjBack, ChunkDataTypes::adjRight, ChunkDataTypes::adjLeft};

                for (int f = 0; f < 6; f++) {
                    adjacentChunks[f] = GetChunkAtPosition(glm::vec3{chunkX, chunkY, chunkZ} + positionOffsets[f]);
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