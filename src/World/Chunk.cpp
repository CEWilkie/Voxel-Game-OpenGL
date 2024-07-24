//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"
#include <glm/gtc/noise.hpp>
#include <utility>

/*
 * SUBCHUNK
 */

SubChunk::SubChunk() = default;

SubChunk::SubChunk(const std::vector<Cube*>& _subCubes) {
    // Move the _cube parameter into the cube var of the SubChunk
    for (auto& cube : _subCubes) subCubes.emplace_back(cube);

    // Fetch min max verticies from cubes
    std::vector<Vertex> vertexArray {};
    for (const auto& cube : subCubes) {
        auto [minVertex, maxVertex] = cube->GetMinMaxGlobalBounds();
        vertexArray.push_back({minVertex});
        vertexArray.push_back({maxVertex});
    }

    // Create culling bounds from cubes
    bounds = std::make_unique<BoxBounds>(GenerateBoxBounds(vertexArray));
}

SubChunk::SubChunk(const std::vector<SubChunk*>& _subChunks) {
    // Assign the child subChunks
    for (auto& chunk : _subChunks) subChunks.emplace_back(chunk);

    // Fetch min max verticies from subChunks
    std::vector<Vertex> vertexArray {};
    for (const auto& chunk : subChunks) {
        auto [minVertex, maxVertex] = chunk->bounds->GetMinMaxVertex();
        vertexArray.push_back({minVertex});
        vertexArray.push_back({maxVertex});
    }

    // Create culling bounds from subChunks
    bounds = std::make_unique<BoxBounds>(GenerateBoxBounds(vertexArray));
}

SubChunk::~SubChunk() {

}


void Chunk::UpdateCubeMatricies() {
    // Used by the parent subChunk, enacts UpdateModelMatrix with a parent transformation matrix
    // Traverse subchunk tree untill reaching the individual cubes to update their model matricies

}

void SubChunk::UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix) {
    // Search through subChunk tree to find cubes
    if (!subChunks.empty()) {
        for (auto& subChunk : subChunks)
            subChunk->UpdateModelMatrix(_parentTransformationMatrix);
    }

    // End of branch, update cubes, goes back to rest of branches
    else if (!subCubes.empty()) {
        for (auto& cube : subCubes)
            cube->UpdateModelMatrix(_parentTransformationMatrix);
    }
}

void SubChunk::CheckCulling(const Camera &_camera) {
    // Bounds are pre-globalised, so utilise a clean transformation
    Transformation t {};
    isCulled = bounds->InFrustrum(_camera.GetCameraFrustrum(), t);

    // If the subchunk is not culled, check further along the tree of subChunks
    if (!isCulled && !subChunks.empty())
        for (auto& subChunk : subChunks) subChunk->CheckCulling(_camera);

    // If the subchunk is not called and the subchunk is an ending node, check the cubes for culling
    if (!isCulled && !subCubes.empty())
        for (auto& cube : subCubes) cube->CheckCulling(_camera);
}

void SubChunk::Display() {
    // If subChunk is culled, then do not attempt to render this chunk, or anything further along the tree
    if (isCulled) return;

    for (auto& subChunk : subChunks) subChunk->Display();
    for (auto& cube : subCubes) cube->Display();
}

/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) {
    CreateHeightMap();

    // Creates blocks in chunk and populates the subChunks
    CreateTerrain();
    if (cubes.empty()) { // big wuh oh
        printf("Failed to Create SubChunks!\n");
        return;
    }

    // Update the model matricies and then create culling bounds for the subChunks
    transformation = std::make_unique<Transformation>();
    transformation->SetPosition(_chunkPosition * (float)chunkSize);
}


void Chunk::Display() {
    for (auto& cube : cubes) cube->Display();
}

void Chunk::CheckCulling(const Camera& _camera) {
    for (auto& cube : cubes) cube->CheckCulling(_camera);
}



void Chunk::CreateHeightMap() {
    // Take a flat xz plane of the chunk, and determine height values for each xz pillar
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int cubeX = x + ((int)chunkOrigin.x * chunkSize);
            int cubeZ = z + ((int)chunkOrigin.z * chunkSize);

            // Get positive noise value
            float simplexNoise = glm::simplex(glm::vec2(cubeX / 64, cubeZ / 64));
            simplexNoise = (simplexNoise + 1) / 2;
            simplexNoise *= 5;

            // Apply to height map
//            heightMap[x + z*chunkSize] += (int)simplexNoise;
        }
    }
}

void Chunk::CreateTerrain() {
    // Temp container for the cubes before being sorted into their subChunks
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
//            int height = heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                int blockY = ((int)chunkOrigin.y * chunkSize) + y;

                glm::vec3 position = {x, y, z};

                // Determine block type
                if (y == chunkSize - 1) cubes.push_back(std::make_unique<Grass>(position));
                else if (y > chunkSize - 4) cubes.push_back(std::make_unique<Dirt>(position));
                else cubes.push_back(std::make_unique<Stone>(position));
            }
        }
    }
}

void Chunk::CreateSubchunks(const std::vector<std::vector<std::vector<Cube*>>>& _xzyCubeContainer) {

}


void Chunk::MoveChunk(glm::vec3 move) {
   // transformation->SetPosition(transformation->GetLocalPosition() + move);
}