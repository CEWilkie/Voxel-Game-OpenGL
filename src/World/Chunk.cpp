//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"
#include <glm/gtc/noise.hpp>
#include <utility>

/*
 * SUBCHUNK
 */

ChunkNode::ChunkNode() = default;

ChunkNode::~ChunkNode() = default;


/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) {
    // Update the model matrix with position
    transformation = std::make_unique<Transformation>();
    transformation->SetPosition(_chunkPosition * (float)chunkSize);

    CreateHeightMap();

    // Creates blocks in chunk and populates the subChunks
    CreateTerrain();
    if (chunkCubes.empty()) { // big wuh oh
        printf("Failed to Create SubChunks!\n");
        return;
    }
}


void Chunk::Display() {
    for (auto& cube : chunkCubes) cube->Display();
}

void Chunk::CheckCulling(const Camera& _camera) {
    for (auto& cube : chunkCubes) cube->CheckCulling(_camera);
}



std::array<int, chunkArea> Chunk::CreateHeightMap() {
    std::array<int, chunkArea> heightMap {};
    // Take a flat xz plane of the chunk, and determine height values for each xz pillar
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int cubeX = x + (int)transformation->GetLocalPosition().x;
            int cubeZ = z + (int)transformation->GetLocalPosition().z;

            // Get positive noise value
            float simplexNoise = glm::simplex(glm::vec2(cubeX / 4, cubeZ / 4));
            simplexNoise = (simplexNoise + 1) / 2;
            simplexNoise *= 5;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)simplexNoise;
        }
    }

    return heightMap;
}

void Chunk::CreateTerrain() {
    // Temp container for the cubes before being sorted into their subChunks
    auto heightMap = CreateHeightMap();

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto height = (float)heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                glm::vec3 blockPos = glm::vec3(x,y,z) + transformation->GetLocalPosition();

                // Determine block type
                if (blockPos.y > height) ;
                else if (blockPos.y == height) chunkCubes.push_back(std::make_unique<Grass>(blockPos));
                else if (blockPos.y > height -4) chunkCubes.push_back(std::make_unique<Dirt>(blockPos));
                else chunkCubes.push_back(std::make_unique<Stone>(blockPos));
            }
        }
    }
}

void Chunk::CreateSubchunks(const std::vector<std::vector<std::vector<Block*>>>& _xzyCubeContainer) {

}


void Chunk::MoveChunk(glm::vec3 move) {
   // transformation->SetPosition(transformation->GetLocalPosition() + move);
}