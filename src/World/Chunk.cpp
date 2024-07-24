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

SubChunk::SubChunk(Cube* _cube) {
    // Move the _cube parameter into the cube var of the SubChunk
    cube.reset(_cube);

    // Create culling bounds for cube
    bounds = std::make_unique<BoxBounds>(GenerateBoxBounds(cube->GetVertexArray()));
}

SubChunk::SubChunk(std::vector<std::unique_ptr<SubChunk>> _subChunks) {
    // Assign the child subChunks and create culling bounds for them.
    subChunks = std::move(_subChunks);
    CreateCullingBounds();
}

SubChunk::~SubChunk() {

}


void SubChunk::UpdateModelMatrix() {
    // Used by the parent subChunk, enacts UpdateModelMatrix with a parent transformation matrix
    // Traverse subchunk tree untill reaching the individual cubes to update their model matricies

    transformation->UpdateModelMatrix();
    UpdateModelMatrix(transformation->GetLocalTransformationMatrix());
}

void SubChunk::UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix) {
    // Search through subChunk tree to find cubes
    if (!subChunks.empty()) {
        for (auto& subChunk : subChunks)
            subChunk->UpdateModelMatrix(_parentTransformationMatrix);
    }

    // If the subchunk is at the end of the tree, it is expected to have a Cube to update instead of further subchunks
    if (cube != nullptr) {
        cube->UpdateModelMatrix(_parentTransformationMatrix);
        return;
    }
}


void SubChunk::CreateCullingBounds() {
    // Take the verticies of the subChunk, and create a bounding box from those
    std::vector<Vertex> subChunkVerticies{};

    if (!subChunks.empty()) {
        for (auto &subChunk: subChunks) {
            // Proceed down the tree untill reaching the final cube node
            subChunk->CreateCullingBounds();

            // Fetch the min, max verticies
            auto minMaxVerticies = subChunk->bounds->GetMinMaxVertex();
            subChunkVerticies.push_back({minMaxVerticies.first});
            subChunkVerticies.push_back({minMaxVerticies.second});
        }

        // Generate Bounding box from the vertex list
        bounds = std::make_unique<BoxBounds>(GenerateBoxBounds(subChunkVerticies));
        auto [mi, ma] = bounds->GetMinMaxVertex();
        printf("minv %f %f %f, maxv %f %f %f\n", mi.x, mi.y, mi.z, ma.x, ma.y, ma.z);
    }
}

void SubChunk::CheckCulling(const Camera &_camera) {
    if (transformation == nullptr) return;

    // Check sphere bounds
    isCulled = bounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation);

    // If the subchunk is not culled, check further along the tree of subChunks
    if (!isCulled && !subChunks.empty()) {
        for (auto& subChunk : subChunks) subChunk->CheckCulling(_camera);
    }
}

void SubChunk::Display() {
    // If subChunk is culled, then do not attempt to render this chunk, or anything further along the tree
    if (isCulled) return;

    for (auto& subChunk : subChunks) subChunk->Display();
    if (cube != nullptr) cube->Display();
}

/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) : SubChunk() {
    CreateHeightMap();

    // Creates blocks in chunk and populates the subChunks
    CreateTerrain();
    if (subChunks.empty()) { // big wuh oh
        printf("Failed to Create SubChunks!\n");
        return;
    }

    // Update the model matricies and then create culling bounds for the subChunks
    transformation->SetPosition(_chunkPosition * (float)chunkSize);
    UpdateModelMatrix();
    SubChunk::CreateCullingBounds();
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
    std::vector<Cube*> cubes {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
//            int height = heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                int blockY = ((int)chunkOrigin.y * chunkSize) + y;

                // Determine block type
                // ...

                // Example stone block
                cubes.push_back(new Stone({x, y, z}));
            }
        }
    }

    CreateSubchunks(cubes);
}

void Chunk::CreateSubchunks(const std::vector<Cube*>& _cubeContainer) {
    if (_cubeContainer.empty()) return;

    // First put all cubes into subchunks
    for (auto& cube : _cubeContainer) {
        auto sc = std::make_unique<SubChunk>(cube);
        subChunks.push_back(std::move(sc));
    }

    printf("SC: %zu\n", subChunks.size());

    // Condense into 8 subchunks (as you move down the tree, each subchunk splits into 8 further sections until reaching
    // the final singular cubes)
    int maxSubChunkIndex = chunkSize;
//    while (subChunks.size() != 8) {
//        for (int x = 0; x < maxSubChunkIndex; x++) {
//            for (int z = 0; z < maxSubChunkIndex; z++) {
//                for (int y = 0; y < maxSubChunkIndex; y++) {
//                    // Fetch subChunk and adjacent subchunks
//                    // subChunks[y + (z*maxSubChunkIndex) + (x*maxSubChunkIndex)];
//
//
//                    printf("x %d y %d z %d\n", x, y, z);
//                }
//            }
//        }
//
//        maxSubChunkIndex /= 2;
//    }

}