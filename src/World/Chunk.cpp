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

ChunkNode::ChunkNode(Block* _nodeBlock, glm::vec3 _blockPos, Chunk* _root) {
    rootChunk = _root;
    nodeBlock = _nodeBlock;

    // Create transformation matrix
    transformation->SetPosition(_blockPos);
    transformation->UpdateModelMatrix();
}

ChunkNode::ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root) {
    rootChunk = _root;
    subNodes = std::move(_subNodes);

    // Set scale of the node
    scale = subNodes.front()->scale * 2;

    // Set position of the node
    glm::vec3 position(subNodes.front()->transformation->GetLocalPosition());
    for (const auto& subNode : subNodes) {
        glm::vec3 subNodePosition = subNode->transformation->GetLocalPosition();
        if (subNodePosition.x < position.x && subNodePosition.y > position.y && subNodePosition.z < position.z)
            position = subNodePosition;
    }
    transformation->SetPosition(position);
    transformation->UpdateModelMatrix();

    // Check if all subnodes are of the same block type
    if (subNodes.front()->isSingleType) {
        Block* firstBlock = subNodes.front()->nodeBlock;
        for (const auto& subNode : subNodes) {
            if (firstBlock == subNode->nodeBlock) continue; // both air blocks -> same type

            // sub node is not of a singular block type, cannot merge
            if (!subNode->isSingleType) {
                isSingleType = false;
                break;
            }

            // Checking air nodes
            if (firstBlock == nullptr && subNode->nodeBlock != nullptr ||
                    firstBlock != nullptr && subNode->nodeBlock == nullptr) {
                isSingleType = false;
                break;
            }

            // sub node block type is not the same as the first node, cannot merge
            if (!BlockType::Compare(subNode->nodeBlock->GetBlockType(), firstBlock->GetBlockType())) {
                isSingleType = false;
                break;
            }
        }
    }
    else isSingleType = false;

    // If they are the same, use their blockData for the chunk
    if (isSingleType) {
        // Drop subNodes
        nodeBlock = subNodes.front()->nodeBlock;
        subNodes.clear();
    }
}
ChunkNode::~ChunkNode() = default;


void ChunkNode::Display() {
    if (!inCamera) return;

    // draw  block
//    if (nodeBlock != nullptr) nodeBlock->Display(transformation.get());
    if (blockMesh != nullptr) blockMesh->DrawMesh(*transformation);
    else for (const auto& node : subNodes) node->Display();
}



void ChunkNode::CreateMaterialMesh() {
    // Traverse node tree until reaching single-type nodes
    for (auto& subNode : subNodes) subNode->CreateMaterialMesh();

    // Not single type or air block, cannot create a material mesh for the node
    if (!isSingleType || nodeBlock == nullptr || nodeBlock->GetBlockType().blockID == AIR) return;

    // Create mesh object for the block type of the node
    blockMesh = std::make_unique<MaterialMesh>(nodeBlock);

    glm::vec3 cOrigin = transformation->GetLocalPosition() - rootChunk->GetPosition();
    glm::vec3 cMax = {cOrigin.x + scale, cOrigin.y + scale, cOrigin.z + scale};

    for (int x = (int)cOrigin.x; x < (int)cMax.x; x++) {
        for (int y = (int)cOrigin.y; y < (int)cMax.y; y++) {
            for (int z = (int)cOrigin.z; z < (int)cMax.z; z++) {
                blockMesh->AddVerticies(nodeBlock->GetFaceVerticies(rootChunk->GetShowingFaces({x,y,z})),
                                        glm::vec3(x,y,z) - cOrigin);
            }
        }
    }

    blockMesh->BindMesh();
}




void ChunkNode::CheckCulling(const Camera& _camera) {
    inCamera = (blockBounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation) == INSIDE);

    // If the node is not culled, check any further nodes
    if (inCamera && !subNodes.empty())
        for (auto& node : subNodes) node->CheckCulling(_camera);
}

void ChunkNode::CheckNodeCulled() {
    // Update Culled Status in subNodes
    for (auto& subNode : subNodes ) subNode->CheckNodeCulled();

    // If any subNodes are not culled, then this parent node is considered not culled
    inCamera = !std::any_of(subNodes.begin(), subNodes.end(), [](const std::unique_ptr<ChunkNode>& subNode){
        return !subNode->inCamera;
    });

    // if there are no subNodes, check the node block's cull status
    if (nodeBlock != nullptr && subNodes.empty()) inCamera = nodeBlock->IsCulled();
}







/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition) {
    // Update the model matrix with position
    transformation = std::make_unique<Transformation>();
    transformation->SetPosition(_chunkPosition * (float)chunkSize);

    // Guarantee Air Block
    uniqueBlocks.emplace_back(CreateBlock({AIR, 0}), 1);

    CreateHeightMap();

    // Measure of chunk creation time
    auto st = SDL_GetTicks64();

    // Creates blocks in chunk and Put blocks into octTree
    CreateNodeTree(CreateTerrain());
    auto et = SDL_GetTicks64();

    printf("CHUNK CREATION : %llu TICKS TAKEN\n", et-st);
    printf("CHUNK HAS %zu UNIQUE BLOCKS\n", uniqueBlocks.size());

    sumTicksTaken += et-st;
    nChunksCreated++;
    averageTicksTaken = sumTicksTaken / nChunksCreated;

    // Measure of face culling
    st = SDL_GetTicks64();
//    CheckExposedFaces();
    rootNode->CreateMaterialMesh();
    et = SDL_GetTicks64();

    printf("CHUNK MESH CREATION : %llu TICKS TAKEN\n", et-st);
}


void Chunk::Display() {
    rootNode->Display();
}





void Chunk::CheckCulling(const Camera& _camera) {
    rootNode->CheckCulling(_camera);
}

std::vector<BLOCKFACE> Chunk::GetHiddenFaces(glm::vec3 _position) const {
    std::vector<BLOCKFACE> hiddenFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
        glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0}, glm::vec3{-1, 0, 0},
        glm::vec3{1,0,0}, glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}};

    Block* checkingBlock = GetBlockFromData(GetBlockDataAtPosition(_position));
    if (checkingBlock == nullptr) return faces; // Could not find block
    if (BlockType::Compare(checkingBlock->GetBlockType(), {AIR, 0})) return faces; // Air block

    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockFromData(GetBlockDataAtPosition(_position + positionOffsets[i]));
        if (blockAtFace == nullptr) { // could not obtain block data
            continue;
        }

        // transparent blocks only show when there is air
        if (checkingBlock->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 1) {
            // Is air
            if (BlockType::Compare(checkingBlock->GetBlockType(), {AIR, 0})) {
                continue;
            }
        }

        // Normal blocks may show if the block on the face is transparent
        else if (blockAtFace->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 1) {
            continue;
        }

        hiddenFaces.push_back(faces[i]);
    }

    return hiddenFaces;
}

std::vector<BLOCKFACE> Chunk::GetShowingFaces(glm::vec3 _position) const {
    std::vector<BLOCKFACE> showingFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
            glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0}, glm::vec3{-1, 0, 0},
            glm::vec3{1,0,0}, glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}};

    // Get block being checked
    Block* checkingBlock = GetBlockFromData(GetBlockDataAtPosition(_position));
    if (checkingBlock == nullptr) return {}; // Could not find block
    if (BlockType::Compare(checkingBlock->GetBlockType(), {AIR, 0})) return {}; // Air block

    // Check for non-transparent block on each face (or non-same transparent block for a transparent block)
    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockFromData(GetBlockDataAtPosition(_position + positionOffsets[i]));
        if (blockAtFace == nullptr) { // could not obtain block data, enable the face
            showingFaces.push_back(faces[i]);
            continue;
        }

        // transparent blocks only show when there is air
        if (checkingBlock->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 1) {
            // Is not air
            if (!BlockType::Compare(blockAtFace->GetBlockType(), {AIR, 0})) {
                continue;
            }
        }

        // Normal blocks may show if the block on the face is transparent
        else if (blockAtFace->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 0) {
            continue;
        }

        showingFaces.push_back(faces[i]);
    }

    return showingFaces;
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
            simplexNoise *= 4;

            // Apply to height map
            heightMap[x + z*chunkSize] += (int)simplexNoise + 32;
        }
    }

    return heightMap;
}

nodeArray Chunk::CreateTerrain() {
    auto heightMap = CreateHeightMap();

    nodeArray chunkBlocks {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto height = (float)heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                glm::vec3 blockPos = glm::vec3(x, y, z) + transformation->GetLocalPosition();

                BlockType newBlockData;

                // Determine block type
                if (blockPos.y < 34 && blockPos.y > height) newBlockData = {WATER, 0};
                else if (blockPos.y > height) newBlockData = {AIR, 0};
                else if (blockPos.y == height) newBlockData = {BLOCKID::GRASS, 0};
                else if (blockPos.y > height - 4) newBlockData = {BLOCKID::DIRT, 0};
                else newBlockData = {BLOCKID::STONE, 0};

                // Check if material is new to the chunk
                if (!std::any_of(uniqueBlocks.begin(), uniqueBlocks.end(),
                                 [&](std::pair<std::unique_ptr<Block>, int> &uniqueBlock) {
                    // Block is unique
                    if (!BlockType::Compare(uniqueBlock.first->GetBlockType(), newBlockData)) return false;

                    // Already exists, increment count
                    uniqueBlock.second += 1;
                    return true;
                })) {
                    // create a new block of the specified type, and create mesh for block
                    uniqueBlocks.emplace_back(CreateBlock(newBlockData), 1);
                    chunkMesh.emplace_back(uniqueBlocks.back().first.get());
                }

                // Add blockID into terrain array
                terrain[x][y][z] = newBlockData;

                // Make leaf node in tree for block
                chunkBlocks[x][y][z] = std::make_unique<ChunkNode>(GetBlockFromData(newBlockData), blockPos, this);
            }
        }
    }

    return chunkBlocks;
}



void Chunk::CreateNodeTree(nodeArray _chunkNodes) {
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




BlockType Chunk::GetBlockDataAtPosition(glm::vec3 _position) const {
    // provided position is outside the bounds of the chunk, defaults to {AIR, 0}
    BlockType outsideChunk{AIR, 0};
    if (_position.x < 0 || _position.x >= chunkSize) return outsideChunk;
    if (_position.y < 0 || _position.y >= chunkSize) return outsideChunk;
    if (_position.z < 0 || _position.z >= chunkSize) return outsideChunk;

    return terrain[(int)_position.x][(int)_position.y][(int)_position.z];
}

Block* Chunk::GetBlockFromData(BlockType _data) const {
    for (const auto& block : uniqueBlocks) {
        if (BlockType::Compare(block.first->GetBlockType(), _data))
            return block.first.get();
    }

    // No block found with specified data
    printf("BLOCK NOT FOUND, ID %d VARIANT %d\n", _data.blockID, _data.variantID);
    return nullptr;
}