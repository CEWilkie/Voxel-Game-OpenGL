//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"
#include <glm/gtc/noise.hpp>
#include <memory>
#include <utility>

/*
 * SUBCHUNK
 */

ChunkNode::ChunkNode(std::unique_ptr<Block> _nodeBlock, glm::vec3 _position) {
    nodeBlock = std::move(_nodeBlock);

    // Set position, scale remains default 1x1x1
    position = _position;
}

ChunkNode::ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes) {
    subNodes = std::move(_subNodes);

    // Set scale of the node
    scale = subNodes.front()->scale * 2.0f;

    // Set position of the node
    position = subNodes.front()->position;
    for (const auto& subNode : subNodes) {
        if (subNode->position.x < position.x && subNode->position.y > position.y && subNode->position.z < position.z)
            position = subNode->position;
    }

    if (subNodes.front()->isSingleType) {

        // Check if all subnodes are of the same block type
        BlockData firstNodeData = subNodes.front()->nodeBlock->GetBlockData();
        for (const auto& subNode : subNodes) {
            // sub node is not of a singular block type, cannot merge
            if (!subNode->isSingleType) {
                isSingleType = false;
                break;
            }

            // sub node block type is not the same as the first node, cannot merge
            if (!BlockData::Compare(subNode->nodeBlock->GetBlockData(), firstNodeData)) {
                isSingleType = false;
                break;
            }
        }
    }
    else isSingleType = false;

    // If they are the same, use their blockData for the chunk
    if (isSingleType) {
        if (nodeBlock == nullptr) nodeBlock = subNodes.front()->nodeBlock;
        nodeBlock->SetScale(scale);
        nodeBlock->SetPositionOrigin({position.x, position.y + scale.y - 1, position.z} );
        nodeBlock->UpdateModelMatrix();
    }
}
ChunkNode::~ChunkNode() = default;


void ChunkNode::Display() {
    if (isCulled) return;

    // Only need to draw one type of block
    if (isSingleType) {
        textureManager->EnableTextureSheet(nodeBlock->GetBlockData().textureSheet);
        nodeBlock->Display();
    }
    else {
        for (const auto& node : subNodes) node->Display();
    }
}

void ChunkNode::CheckCulling(const Camera& _camera) {
    Transformation t;
    t.SetScale(scale);
    t.SetPosition(position);
    t.UpdateModelMatrix();
    isCulled = !blockBounds->InFrustrum(_camera.GetCameraFrustrum(), t);
}

ChunkNode ChunkNode::CreateNodeTree(std::array<std::array<std::array<ChunkNode*, chunkSize>, chunkSize>, chunkSize> _blockNodes) {
    // First create a 3d array of ending nodes


    int size = chunkSize;


}



/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) {
    // Update the model matrix with position
    transformation = std::make_unique<Transformation>();
    transformation->SetPosition(_chunkPosition * (float)chunkSize);

    CreateHeightMap();

    // Creates blocks in chunk and Put blocks into octTree
    CreateNodeTree(CreateTerrain());
}


void Chunk::Display() {
    rootNode->Display();
}

void Chunk::CheckCulling(const Camera& _camera) {
    rootNode->CheckCulling(_camera);
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
            simplexNoise *= 3;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)simplexNoise + 5;
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

                std::unique_ptr<Block> newBlock {};

                // Determine block type
                if (blockPos.y > height) newBlock = std::make_unique<Air>(blockPos);
                else if (blockPos.y == height) newBlock = std::make_unique<Grass>(blockPos);
                else if (blockPos.y > height -4) newBlock = (std::make_unique<Dirt>(blockPos));
                else newBlock = (std::make_unique<Stone>(blockPos));

                chunkBlocks[x][y][z] = std::make_unique<ChunkNode>(std::move(newBlock), blockPos);
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
                    _chunkNodes[x][y][z] = std::make_unique<ChunkNode>(std::move(subNodes));
                    nodes +=1;
                }
        nodesToTraverse = nodesToTraverse / 2;
        printf("NODES: %d\n", nodes);
    }

    rootNode = std::move(_chunkNodes[0][0][0]);

}


void Chunk::MoveChunk(glm::vec3 move) {
   // transformation->SetPosition(transformation->GetLocalPosition() + move);
}

