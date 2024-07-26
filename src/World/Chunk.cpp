//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"

#include <glm/gtc/noise.hpp>
#include <memory>
#include <utility>

#include "../Blocks/CreateBlock.h"

/*
 * SUBCHUNK
 */

ChunkNode::ChunkNode(BlockData _nodeBlockData, glm::vec3 _position, Chunk* _root) {
    rootChunk = _root;
    nodeBlockData = _nodeBlockData;

    // Set position, scale remains default 1x1x1
    position = _position;

    // Create transformation matrix
    transformation->SetPosition(position);
    transformation->UpdateModelMatrix();
}

ChunkNode::ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root) {
    rootChunk = _root;
    subNodes = std::move(_subNodes);

    // Set scale of the node
    scale = subNodes.front()->scale * 2.0f;

    // Set position of the node
    position = subNodes.front()->position;
    for (const auto& subNode : subNodes) {
        if (subNode->position.x < position.x && subNode->position.y > position.y && subNode->position.z < position.z)
            position = subNode->position;
    }
    position.y += scale.y / 2;


    // Check if all subnodes are of the same block type
    if (subNodes.front()->isSingleType) {
        BlockData firstNodeData = subNodes.front()->nodeBlockData;
        for (const auto& subNode : subNodes) {
            // sub node is not of a singular block type, cannot merge
            if (!subNode->isSingleType) {
                isSingleType = false;
                break;
            }

            // sub node block type is not the same as the first node, cannot merge
            if (!BlockData::Compare(subNode->nodeBlockData, firstNodeData)) {
                isSingleType = false;
                break;
            }
        }
    }
    else isSingleType = false;

    // If they are the same, use their blockData for the chunk
    if (isSingleType) {
        nodeBlockData = subNodes.front()->nodeBlockData;
        transformation->SetScale(scale);
        transformation->SetPosition(position);
        transformation->UpdateModelMatrix();

        // drop subnodes
        subNodes.clear();
    }
}
ChunkNode::~ChunkNode() = default;


void ChunkNode::Display() {
    if (isCulled) return;

    // Only need to draw one type of block
    if (isSingleType) {
        Block* displayBlock = rootChunk->GetBlockFromData(nodeBlockData);
        if (displayBlock != nullptr)
            displayBlock->Display(*transformation);
    }
    else {
        for (const auto& node : subNodes) node->Display();
    }
}

void ChunkNode::CheckCulling(const Camera& _camera) {
    isCulled = !blockBounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation);

    // If the node is not culled, check any further nodes
    if (!isCulled && !subNodes.empty())
        for (auto& node : subNodes) node->CheckCulling(_camera);
}




/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) {
    // Update the model matrix with position
    transformation = std::make_unique<Transformation>();
    transformation->SetPosition(_chunkPosition * (float)chunkSize);

    CreateHeightMap();

    // Measure of chunk creation time
    auto st = SDL_GetTicks64();

    // Creates blocks in chunk and Put blocks into octTree
    CreateNodeTree(CreateTerrain());
    auto et = SDL_GetTicks64();

    printf("CHUNK CREATION : %llu TICKS TAKEN\n", et-st);
}


void Chunk::Display() {
    rootNode->Display();
}

void Chunk::CheckCulling(const Camera& _camera) {
    rootNode->CheckCulling(_camera);
}

Block *Chunk::GetBlockFromData(BlockData _data) {
    for (auto& block : uniqueBlocks) {
        if (BlockData::Compare(block->GetBlockData(), _data)) return block.get();
    }

    return nullptr;
}



std::array<int, chunkArea> Chunk::CreateHeightMap() {
    std::array<int, chunkArea> heightMap {};
    // Take a flat xz plane of the chunk, and determine height values for each xz pillar
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int cubeX = x + (int)transformation->GetLocalPosition().x;
            int cubeZ = z + (int)transformation->GetLocalPosition().z;

            // Get positive noise value
            float simplexNoise = glm::simplex(glm::vec2(cubeX / 16.0, cubeZ / 16.0));
            simplexNoise = (simplexNoise + 1) / 2;
            simplexNoise *= 3;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)simplexNoise + 32;
        }
    }

    return heightMap;
}

std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> Chunk::CreateTerrain() {
    auto heightMap = CreateHeightMap();

    std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> chunkBlocks {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto height = (float)heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                glm::vec3 blockPos = glm::vec3(x,y,z) + transformation->GetLocalPosition();

                BlockData newBlockData;

                // Determine block type
                if (blockPos.y > height) newBlockData = {BLOCKID::AIR, 0};
                else if (blockPos.y == height) newBlockData = {BLOCKID::GRASS, 0};
                else if (blockPos.y > height -4) newBlockData = {BLOCKID::DIRT, 0};
                else newBlockData = {BLOCKID::STONE, 0};

                // Check if material is new to the chunk
                if (!std::any_of(uniqueBlocks.begin(), uniqueBlocks.end(), [&](const std::unique_ptr<Block>& uniqueBlock){
                    return BlockData::Compare(uniqueBlock->GetBlockData(), newBlockData);
                })) {
                    // new material, add to block list
                    uniqueBlocks.push_back(CreateBlock(newBlockData.blockID, newBlockData.variantID));
                }

                chunkBlocks[x][y][z] = std::make_unique<ChunkNode>(newBlockData, blockPos, this);
            }
        }
    }

    return chunkBlocks;
}



void Chunk::CreateNodeTree(std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> _chunkNodes) {
    int nodesToTraverse = chunkSize;
    int offset;
    int nodes;

    while (nodesToTraverse != 1) {
        nodes = 0;
        offset = chunkSize/nodesToTraverse;
        for (int x = 0; x < chunkSize; x+=2*offset)
            for (int y = 0; y < chunkSize; y+=2*offset)
                for (int z = 0; z < chunkSize; z+=2*offset) {
                    std::vector<std::unique_ptr<ChunkNode>> subNodes {};
                    subNodes.push_back(std::move(_chunkNodes[x][y][z]));
                    subNodes.push_back(std::move(_chunkNodes[x+offset][y][z]));
                    subNodes.push_back(std::move(_chunkNodes[x][y][z+offset]));
                    subNodes.push_back(std::move(_chunkNodes[x+offset][y][z+offset]));

                    subNodes.push_back(std::move(_chunkNodes[x][y+offset][z]));
                    subNodes.push_back(std::move(_chunkNodes[x+offset][y+offset][z]));
                    subNodes.push_back(std::move(_chunkNodes[x][y+offset][z+offset]));
                    subNodes.push_back(std::move(_chunkNodes[x+offset][y+offset][z+offset]));

                    // Create new node
                    _chunkNodes[x][y][z] = std::make_unique<ChunkNode>(std::move(subNodes), this);
                    nodes +=1;
                }
        nodesToTraverse = nodesToTraverse / 2;
    }

    rootNode = std::move(_chunkNodes[0][0][0]);

}


void Chunk::MoveChunk(glm::vec3 move) {
   // transformation->SetPosition(transformation->GetLocalPosition() + move);
}

