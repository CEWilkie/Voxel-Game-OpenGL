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
    if (nodeBlock != nullptr) nodeBlock->SetTransformation(transformation.get());
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
            if (!BlockData::Compare(subNode->nodeBlock->GetBlockData(), firstBlock->GetBlockData())) {
                isSingleType = false;
                break;
            }
        }
    }
    else isSingleType = false;

    // If they are the same, use their blockData for the chunk
    if (isSingleType) {
        nodeBlock = subNodes.front()->nodeBlock;
        if (nodeBlock != nullptr) nodeBlock->SetTransformation(transformation.get());

        // drop subnodes
        subNodes.clear();
    }
}
ChunkNode::~ChunkNode() = default;


void ChunkNode::Display() {
    if (isCulled) return;

    // draw  block
//    if (nodeBlock != nullptr) nodeBlock->Display();
    if (blockMesh != nullptr) blockMesh->DrawMesh(*transformation);
    else for (const auto& node : subNodes) node->Display();
}



void ChunkNode::CreateMaterialMesh() {
    // Traverse node tree until reaching single-type nodes
    for (auto& subNode : subNodes) subNode->CreateMaterialMesh();

    // Not single type or air block, cannot create a material mesh for the node
    if (!isSingleType || nodeBlock == nullptr || nodeBlock->GetBlockData().blockID == AIR) return;

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
    isCulled = (blockBounds->InFrustrum(_camera.GetCameraFrustrum(), *transformation) == OUTSIDE);

    // If the node is not culled, check any further nodes
    if (!isCulled && !subNodes.empty())
        for (auto& node : subNodes) node->CheckCulling(_camera);
}

void ChunkNode::CheckNodeCulled() {
    // Update Culled Status in subNodes
    for (auto& subNode : subNodes ) subNode->CheckNodeCulled();

    // If any subNodes are not culled, then this parent node is considered not culled
    isCulled = !std::any_of(subNodes.begin(), subNodes.end(), [](const std::unique_ptr<ChunkNode>& subNode){
        return !subNode->isCulled;
    });

    // if there are no subNodes, check the node block's cull status
    if (nodeBlock != nullptr && subNodes.empty()) isCulled = nodeBlock->IsCulled();
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
    sumTicksTaken += et-st;
    nChunksCreated++;
    averageTicksTaken = sumTicksTaken / nChunksCreated;

    // Measure of face culling
    st = SDL_GetTicks64();
//    CheckExposedFaces();
    rootNode->CreateMaterialMesh();
    et = SDL_GetTicks64();

    printf("CHUNK FACE CULLING : %llu TICKS TAKEN\n", et-st);
}


void Chunk::Display() {
//    for (auto& mesh : chunkMesh) mesh.DrawMesh(*transformation);

//    for (int x = 0; x < chunkSize; x++)
//        for (int y = 0; y < chunkSize; y++)
//            for (int z = 0; z < chunkSize; z++)
//                terrain[x][y][z]->Display();

    rootNode->Display();
}

Block* Chunk::GetBlockFromData(BlockData _data) {
//    for (const auto& block : uniqueBlocks) {
//        if (BlockData::Compare(block.first->GetBlockData(), _data)) return block.first.get();
//    }

    return nullptr;
}




void Chunk::CheckCulling(const Camera& _camera) {
    rootNode->CheckCulling(_camera);
}

Block* Chunk::GetBlockAtPosition(glm::vec3 _position) {
    if (_position.x < 0 || _position.x >= chunkSize) return nullptr;
    if (_position.y < 0 || _position.y >= chunkSize) return nullptr;
    if (_position.z < 0 || _position.z >= chunkSize) return nullptr;

    return terrain[(int)_position.x][(int)_position.y][(int)_position.z].get();
}

std::vector<BLOCKFACE> Chunk::GetHiddenFaces(glm::vec3 _position) {
    std::vector<BLOCKFACE> hiddenFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
        glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0}, glm::vec3{-1, 0, 0},
        glm::vec3{1,0,0}, glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}};

    Block* checkingBlock = GetBlockAtPosition(_position);

    for (int i = 0; i < faces.size(); i++) {
        Block* block = GetBlockAtPosition(_position + positionOffsets[i]);

        // Edge of chunk, so assume the face is visible
        if (block == nullptr) continue;

        // Block found
        if (checkingBlock->GetBlockData().blockID == WATER) {
            if (!BlockData::Compare(block->GetBlockData(), {AIR, 0}))
                hiddenFaces.push_back(faces[i]);
        }
        else {
            if (!BlockData::Compare(block->GetBlockData(), {AIR, 0}) &&
                !BlockData::Compare(block->GetBlockData(), {WATER, 0}))
                hiddenFaces.push_back(faces[i]);
        }

    }

    return hiddenFaces;
}

std::vector<BLOCKFACE> Chunk::GetShowingFaces(glm::vec3 _position) {
    std::vector<BLOCKFACE> showingFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
            glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0}, glm::vec3{-1, 0, 0},
            glm::vec3{1,0,0}, glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}};

    Block* checkingBlock = GetBlockAtPosition(_position);
    if (checkingBlock == nullptr) return {}; // Air blocks dont show

    for (int i = 0; i < faces.size(); i++) {
        Block* block = GetBlockAtPosition(_position + positionOffsets[i]);

        // Edge of chunk or air --> assume the face is visible
        if (block == nullptr) {
            showingFaces.push_back(faces[i]);
            continue;
        }

        if (BlockData::Compare(checkingBlock->GetBlockData(), {WATER, 0})) {
            continue;
        }
        else {
            if (BlockData::Compare(block->GetBlockData(), {WATER, 0}))
                showingFaces.push_back(faces[i]);
        }

    }

    return showingFaces;
}

void Chunk::CheckExposedFaces() {
    for (int x = 0; x < chunkSize; x++)
        for (int y = 0; y < chunkSize; y++)
            for (int z = 0; z < chunkSize; z++) {
                if (terrain[x][y][z] == nullptr) continue;
                if (terrain[x][y][z]->GetBlockData().blockID != AIR){
                    terrain[x][y][z]->HideFaces(GetHiddenFaces({x, y, z}));
                }
            }

    rootNode->CheckNodeCulled();
}

void Chunk::CreateChunkMesh() {

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

std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> Chunk::CreateTerrain() {
    auto heightMap = CreateHeightMap();

    std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> chunkBlocks {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto height = (float)heightMap[x + z*chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                glm::vec3 blockPos = glm::vec3(x, y, z) + transformation->GetLocalPosition();

                BlockData newBlockData;

                // Determine block type
                if (blockPos.y < 34 && blockPos.y > height) newBlockData = {WATER, 0};
                else if (blockPos.y > height) newBlockData = {AIR, 0};
                else if (blockPos.y == height) newBlockData = {BLOCKID::GRASS, 0};
                else if (blockPos.y > height - 4) newBlockData = {BLOCKID::DIRT, 0};
                else newBlockData = {BLOCKID::STONE, 0};

                auto newBlock = CreateBlock(newBlockData);
                if (newBlockData.blockID != AIR) {
                    // Check if material is new to the chunk
                    if (!std::any_of(uniqueBlocks.begin(), uniqueBlocks.end(), [&](std::pair<BlockData, int> &uniqueBlock) {
                        if (BlockData::Compare(uniqueBlock.first, newBlockData)) {
                            uniqueBlock.second += 1;
                            return true;
                        }
                        return false;
                    })) {
                        // new material, add to blockData list
                        uniqueBlocks.emplace_back(newBlockData, 1);
                        chunkMesh.emplace_back(newBlock.get());
                    }

                    terrain[x][y][z] = std::move(newBlock);
                }
                chunkBlocks[x][y][z] = std::make_unique<ChunkNode>(terrain[x][y][z].get(),
                                                                   blockPos, this);
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

