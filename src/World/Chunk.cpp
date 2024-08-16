//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"

#include <glm/gtc/noise.hpp>
#include <memory>
#include <utility>

#include "../Blocks/CreateBlock.h"
#include "../GlobalStates.h"

/*
 * SUBCHUNK
 */

ChunkNode::ChunkNode(Block* _nodeBlock, glm::vec3 _blockPos, Chunk* _root) {
    rootChunk = _root;
    nodeBlock = _nodeBlock;

    // set position
    position = _blockPos;
}

ChunkNode::ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root) {
    rootChunk = _root;
    subNodes = std::move(_subNodes);

    // Set scale of the node
    scale = subNodes.front()->scale * 2;

    // Set position of the node
    position = subNodes.front()->position;
    for (const auto& subNode : subNodes) {
        glm::vec3 subNodePosition = subNode->position;
        if (subNodePosition.x < position.x && subNodePosition.y > position.y && subNodePosition.z < position.z)
            position = subNodePosition;
    }

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



void ChunkNode::UpdateMaterialMesh(MaterialMesh* _mesh) {
    // Traverse node tree until reaching single-type nodes
    for (auto& subNode : subNodes)
        subNode->UpdateMaterialMesh(_mesh);

    // Not single type or air block, cannot create a material mesh for the node
    if (!isSingleType || nodeBlock == nullptr || nodeBlock->GetBlockType().blockID == AIR) return;

    // Not matching type of the mesh
    if (!BlockType::Compare(_mesh->GetBlock()->GetBlockType(), nodeBlock->GetBlockType())) return;

    glm::vec3 cOrigin = position - rootChunk->GetPosition() * (float)chunkSize;
    glm::vec3 cMax = {cOrigin.x + scale, cOrigin.y + scale, cOrigin.z + scale};

    for (int x = (int)cOrigin.x; x < (int)cMax.x; x++) {
        for (int y = (int)cOrigin.y; y < (int)cMax.y; y++) {
            for (int z = (int)cOrigin.z; z < (int)cMax.z; z++) {
                _mesh->AddVerticies(nodeBlock->GetFaceVerticies(rootChunk->GetShowingFaces({x,y,z})),
                                        glm::vec3(x,y,z));
            }
        }
    }
}








/*
 * CHUNK
 */

Chunk::Chunk(const glm::vec3& _chunkPosition, ChunkData _chunkData) {
    // Update the chunkPosition and chunkBounds transformations
    displayTransformation.SetPosition(_chunkPosition * (float)chunkSize);
    displayTransformation.UpdateModelMatrix();

    cullingTransformation.SetPosition(_chunkPosition * (float)chunkSize);
    cullingTransformation.UpdateModelMatrix();

    // Set chunk position
    chunkPosition = _chunkPosition;

    // Create bounding box for the chunk (assume max 16x16x16 volume)
    boxBounds = std::move(GenerateBoxBounds({{glm::vec3(0,0,0)},
                                             {glm::vec3(chunkSize,chunkSize,chunkSize)}}));

    // Guarantee Air Block
    uniqueBlocks.emplace_back(CreateBlock({AIR, 0}), 1);

    // Set chunkData
    chunkData = _chunkData;
}

Chunk::~Chunk() = default;



/*
 * Displays only the block meshes which have no transparent elements
 */

void Chunk::DisplaySolid() {
    if (!inCamera) return;

    if (USE_WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw only the blocks that are solid
    for (const auto& mesh : blockMeshes) {
        if (mesh->GetBlock()->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 1) continue;
        mesh->DrawMesh(displayTransformation);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



/*
 * Displays only the block meshes which have transparent elements
 */

void Chunk::DisplayTransparent() {
    if (!inCamera) return;

    if (USE_WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw only the blocks that are transparent
    for (const auto& mesh : blockMeshes) {
        if (mesh->GetBlock()->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 0) continue;
        mesh->DrawMesh(displayTransformation);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}






/*
 * Create the initial meshes of the blocks in the chunk
 */

void Chunk::CreateBlockMeshes() {
    // Create block meshes for each unique block
    auto st = SDL_GetTicks64();
    for (const auto& block : uniqueBlocks) {
        blockMeshes.push_back(std::make_unique<MaterialMesh>(block.first.get()));
    }

    // Apply the verticies for all non-air meshes and bind them
    CreateChunkMeshes();
    for (auto& mesh : blockMeshes) mesh->BindMesh();

    auto et = SDL_GetTicks64();

    meshSumTicksTaken += et - st;
    nMeshesCreated++;
    meshAvgTicksTaken = meshSumTicksTaken / nMeshesCreated;
}



/*
 * Updates the mesh of a block at a given position
 */

void Chunk::UpdateMeshAtPosition(glm::vec3 _blockPos) {
    Block* meshBlock = GetBlockAtPosition(_blockPos, 0);
    UpdateBlockMesh(meshBlock);
}



/*
 * Updates the mesh of a given block. This block must belong to the unique blocks of the chunk, otherwise the blockMesh
 * will not be obtained, even if it is the same blockType being updated.
 */

void Chunk::UpdateBlockMesh(Block* _meshBlock) {
    MaterialMesh* blockMesh = GetMeshFromBlock(_meshBlock);
    if (blockMesh == nullptr || blockMesh->GetBlock()->GetBlockType().blockID == AIR) return;

    blockMesh->ResetVerticies();

    for (int x = 0; x < chunkSize; x++) {
        for (int y = 0; y < chunkSize; y++) {
            for (int z = 0; z < chunkSize; z++) {
                Block* block = GetBlockAtPosition({x,y,z}, 0);

                if (BlockType::Compare(block->GetBlockType(), _meshBlock->GetBlockType())) {
                    blockMesh->AddVerticies(block->GetFaceVerticies(GetShowingFaces({x,y,z})), {x,y,z});
                }
            }
        }
    }

    blockMesh->BindMesh();
}

/*
 * Goes through all positions within the chunk and adds the visible verticies of blocks to their corresponding meshes.
 * Affects all meshes except any potential air mesh, and assumes meshes will be reset prior to function call and bound
 * after function call.
 */

void Chunk::CreateChunkMeshes() {
    for (int x = 0; x < chunkSize; x++) {
        for (int y = 0; y < chunkSize; y++) {
            for (int z = 0; z < chunkSize; z++) {
                Block* block = GetBlockAtPosition({x,y,z}, 0);
                MaterialMesh* blockMesh = GetMeshFromBlock(block);
                if (blockMesh == nullptr || blockMesh->GetBlock()->GetBlockType().blockID == AIR) continue;

                std::vector<Vertex> verticies = block->GetFaceVerticies(GetShowingFaces({x,y,z}));
                blockMesh->AddVerticies(verticies, {x,y,z});
            }
        }
    }
}



/*
 * Returns the FaceIDs of the obscured faces of a block at a given position. Faces should be fully obscured to be
 * considered hidden.
 */

std::vector<BLOCKFACE> Chunk::GetHiddenFaces(glm::vec3 _blockPos) const {
    std::vector<BLOCKFACE> hiddenFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
            glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0}, glm::vec3{-1, 0, 0},
            glm::vec3{1,0,0}, glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}};

    Block* checkingBlock = GetBlockAtPosition(_blockPos, 0);
    if (checkingBlock == nullptr) return faces; // Could not find block
    if (BlockType::Compare(checkingBlock->GetBlockType(), {AIR, 0})) return faces; // Air block

    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockAtPosition(_blockPos + positionOffsets[i], 0);
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



/*
 * Returns the FaceIDs of the visible faces of a block at a given position. Faces are considered visible unless fully
 * obscured.
 */
std::vector<BLOCKFACE> Chunk::GetShowingFaces(glm::vec3 _blockPos) const {
    std::vector<BLOCKFACE> showingFaces {};
    std::vector<BLOCKFACE> faces {TOP, BOTTOM, FRONT, BACK, RIGHT, LEFT};
    std::vector<glm::vec3> positionOffsets {
            dirTop, dirBottom, dirFront,
            dirBack, dirRight, dirLeft};

    // Get block being checked
    Block* checkingBlock = GetBlockAtPosition(_blockPos, 0);
    if (checkingBlock == nullptr) return {}; // Could not find block

    // Check for non-transparent block on each face (or non-same transparent block for a transparent block)
    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockAtPosition(_blockPos + positionOffsets[i], 0);
        if (blockAtFace == nullptr) { // could not obtain block data
//            showingFaces.push_back(faces[i]);
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

MaterialMesh* Chunk::GetMeshFromBlock(Block* _block) {
    if (_block == nullptr) return nullptr;

    // Check for mesh in unique meshes vector
    for (const auto& mesh : blockMeshes) {
        if (mesh->GetBlock() == _block) return mesh.get();
    }

    // No mesh found for specified block -> create a new mesh for that block
    blockMeshes.push_back(std::make_unique<MaterialMesh>(_block));
    return blockMeshes.back().get();
}



/*
 * Updates inCamera value with if / not the chunk is within the player camera's view frustrum
 */

void Chunk::CheckCulling(const Camera& _camera) {
    inCamera = boxBounds->InFrustrum(_camera.GetCameraFrustrum(), cullingTransformation);
}



/*
 * Generates the chunk's blocks into the 3d terrain array using stored data maps
 */

void Chunk::GenerateChunk() {
    // Populate the terrain array with blocks
    CreateTerrain();

    // Add in structures

}



/*
 * Generates the terrain specifically
 */

void Chunk::CreateTerrain() {
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto hmTopLevel = chunkData.heightMap[x + z * chunkSize];

            for (int y = 0; y < chunkSize; y++) {
                glm::vec3 blockPos = glm::vec3(x, y, z) + (chunkPosition * (float)chunkSize);

                BlockType newBlockData = chunkData.biome->GetBlockType(hmTopLevel, blockPos.y);

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
                }

                // Add blockID into terrain array
                terrain[x][y][z] = GetBlockFromData(newBlockData);
            }
        }
    }

    generated = true;
}



/*
 * Set the adjacent chunks in each direction to this chunk
 */

void Chunk::SetAdjacentChunks(const std::array<Chunk*, 10> &_chunks) {
    // +y, -y, -x, -x-z, -z, -z+x, +x, +x+z, +z, +z-x
    adjacentChunks = _chunks;
}



/*
 * Set the block at the given block position to air, and update the meshes of the broken block, and surrounding blocks
 */

void Chunk::BreakBlockAtPosition(glm::vec3 _blockPos) {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, 0);
    if (blockChunk == nullptr) return;

    // Fetch block being broken, if this is somehow null theres bigger issues at play
    Block* block = blockChunk->GetBlockAtPosition(_blockPos, 0);
    if (block == nullptr) return;

    // Set the block being broken to AIR 0
    blockChunk->SetBlockAtPosition(_blockPos, 0, {AIR, 0});

    // update blocks mesh
    MaterialMesh* blockMesh = blockChunk->GetMeshFromBlock(block);
    if (blockMesh != nullptr) blockChunk->UpdateBlockMesh(block);

    // Update the meshes of each block adjacent to the block just broken
    std::array<glm::vec3, 7> blockPositions {_blockPos + dirTop, _blockPos + dirBottom, _blockPos + dirLeft,
                                   _blockPos + dirRight, _blockPos + dirFront, _blockPos + dirBack};

    for (auto& blockPosition : blockPositions) {
        Chunk* chunkAtPosition = blockChunk->GetChunkAtPosition(blockPosition, 0);
        if (chunkAtPosition == nullptr) continue;

        chunkAtPosition->UpdateBlockMesh(chunkAtPosition->GetBlockAtPosition(blockPosition, 0));
    }

}



/*
 * Set the block at the given position to the given block type. Will construct a new unique block instance if the block
 * is new to the chunk. Then updates the meshes of the newly placed block and adjacent blocks.
 */

void Chunk::PlaceBlockAtPosition(glm::vec3 _blockPos, BlockType _blockType) {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, 0);
    if (blockChunk == nullptr) return;

    // Get unique block instance and set terrain at position to block
    Block* block = blockChunk->GetBlockFromData(_blockType);
    if (block == nullptr) return;

    terrain[(int)_blockPos.x][(int)_blockPos.y][(int)_blockPos.z] = GetBlockFromData(_blockType);

    // update blocks mesh
    MaterialMesh* blockMesh = blockChunk->GetMeshFromBlock(block);
    if (blockMesh != nullptr) blockChunk->UpdateBlockMesh(block);

    // Update the meshes of each block adjacent to the block just placed
    std::array<glm::vec3, 7> blockPositions {_blockPos + dirTop, _blockPos + dirBottom, _blockPos + dirLeft,
                                             _blockPos + dirRight, _blockPos + dirFront, _blockPos + dirBack};

    for (auto& blockPosition : blockPositions) {
        Chunk* chunkAtPosition = blockChunk->GetChunkAtPosition(blockPosition, 0);
        if (chunkAtPosition == nullptr) continue;

        chunkAtPosition->UpdateBlockMesh(chunkAtPosition->GetBlockAtPosition(blockPosition, 0));
    }


}



/*
 * Sets the block at given position with the given type. blockPos is assumed to have values within 0 - 15
 */

void Chunk::SetChunkBlockAtPosition(const glm::vec3 &_blockPos, const BlockType& _blockType) {
    terrain[(int)_blockPos.x][(int)_blockPos.y][(int)_blockPos.z] = GetBlockFromData(_blockType);
}

/*
 * Obtains the chunk that the provided position is within, and then sets the block in that chunk to the specified type
 */

void Chunk::SetBlockAtPosition(glm::vec3 _blockPos, int _depth, const BlockType& _blockType) {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, _depth);
    if (blockChunk == nullptr) return;

    blockChunk->SetChunkBlockAtPosition(_blockPos, _blockType);
}



/*
 * Fetches block at position. Assumes provided position values are within 0 - 15.
 */

Block* Chunk::GetChunkBlockAtPosition(const glm::vec3 &_blockPos) {
    return terrain[(int)_blockPos.x][(int)_blockPos.y][(int)_blockPos.z];
}

/*
 * Obtains the chunk that the provided position is within, and then returns the block in the chunk at that position
 */

Block* Chunk::GetBlockAtPosition(glm::vec3 _blockPos, int _depth) const {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, _depth);
    if (blockChunk == nullptr) return nullptr;

    return blockChunk->GetChunkBlockAtPosition(_blockPos);
}





/*
 * Returns a pointer to a unique block instance in the chunk. If the provided data does not correlate to a unique block
 * then a new block is created and added, and a pointer to that returned.
 */

Block* Chunk::GetBlockFromData(const BlockType& _blockType) {
    for (const auto& block : uniqueBlocks) {
        if (BlockType::Compare(block.first->GetBlockType(), _blockType))
            return block.first.get();
    }

    // No block found with specified data, so it is new to the chunk -> add to uniqueBlocks
    uniqueBlocks.emplace_back(CreateBlock(_blockType), 1);
    return uniqueBlocks.back().first.get();
}



/*
 * Get the value of the topmost y position of the chunks blocks at the given block position
 */

float Chunk::GetTopLevelAtPosition(glm::vec3 _blockPos, float _radius) const {
    float topLevel = -20;

    // if position y is 0.8 or above, round to ciel
    float y = (_blockPos.y - floorf(_blockPos.y) >= 0.8f) ? roundf(_blockPos.y) : floorf(_blockPos.y);

    // to 0.01 precision, convert to int to *maybe* stop some imprecision issues with looping through with floats
    for (int x = int(100.0 * (_blockPos.x - _radius)); x <= int(100.0 * (_blockPos.x + _radius)); x += int(100.0 * _radius)) {
        for (int z = int(100.0 * (_blockPos.z - _radius)); z <= int(100.0 * (_blockPos.z + _radius)); z += int(100.0 * _radius)) {
            // Convert back to float position of block relative to chunk
            glm::vec3 position{x/100.0, y, z/100.0};
            Block* block = GetBlockAtPosition(position, 0);

            // If no block found / air, or if it is a liquid (ie: water) then do not apply topLevel
            if (block == nullptr || block->GetBlockType().blockID == AIR) continue;
            if (block->GetAttributeValue(BLOCKATTRIBUTE::LIQUID) > 0) continue;

            // blockHeight + y in chunk + chunkHeight
            float blockTL = 1.0f + y + chunkPosition.y*(float)chunkSize;
            if (blockTL > topLevel) topLevel = blockTL;

        }
    }

    return topLevel;
}



/*
 * Get the distance from the provided position to the next solid surface in a given direction.
 */

float Chunk::GetDistanceToBlockFace(glm::vec3 _blockPos, glm::vec3 _direction, float _radius) const {
    if (_direction == glm::vec3{0,0,0}) return 0;

    // get block in direction player is checking, if no block is found, assume no obstructions for the next
    // 2 blocks (to prevent stop-starting player movement if they move faster than 1 block/second)
    // also applies for air or liquid blocks
    Block* block = GetBlockAtPosition(_blockPos + _direction, 0);
    if (block == nullptr || block->GetBlockType().blockID == AIR || block->GetAttributeValue(BLOCKATTRIBUTE::LIQUID) > 0) {
        if (_direction.x != 0) return floorf(_blockPos.x) + _direction.x * 2.0f;
        if (_direction.y != 0) return floorf(_blockPos.y) + _direction.y * 2.0f;
        if (_direction.z != 0) return floorf(_blockPos.z) + _direction.z * 2.0f;
    }

    BLOCKFACE face {};
    std::vector<Vertex> faceVerticies {};
    float minZ {0}, maxZ {1};
    float minX {0}, maxX {1};

    if (_direction == dirFront) face = BACK;
    if (_direction == dirBack) face = FRONT;
    if (_direction == dirLeft) face = RIGHT;
    if (_direction == dirRight) face = LEFT;

    // Get min and max face verticies for x and z position
    faceVerticies = block->GetFaceVerticies({face});
    for (auto& vertex : faceVerticies) {
        vertex.position += glm::floor(_blockPos) + _direction;
        if (vertex.position.z < minZ) minZ = vertex.position.z;
        if (vertex.position.z > maxZ) maxZ = vertex.position.z;

        if (vertex.position.x < minX) minX = vertex.position.x;
        if (vertex.position.x > maxX) maxX = vertex.position.x;
    }

    // Player is only blocked by the block if their position +- radius is within min and max bounds
    if (_direction.x != 0) {
        if ((_blockPos.z + _radius >= minZ && _blockPos.z + _radius <= maxZ) ||
            (_blockPos.z - _radius >= minZ && _blockPos.z - _radius <= maxZ)) {
            if (_direction == dirFront) return maxX;
            else return minX + floorf(_blockPos.x) + _direction.x;
        }
    }

    if (_direction.z != 0) {
        if ((_blockPos.x + _radius >= minX && _blockPos.x + _radius <= maxX) ||
            (_blockPos.x - _radius >= minX && _blockPos.x - _radius <= maxX)) {
            if (_direction == dirLeft) return maxZ;
            else return minZ + floorf(_blockPos.z) + _direction.z;
        }
    }

    // Player not within min and max bounds, so not blocked
    if (_direction.x != 0) return floorf(_blockPos.x) + _direction.x * 2.0f;
    if (_direction.y != 0) return floorf(_blockPos.y) + _direction.y * 2.0f;
    if (_direction.z != 0) return floorf(_blockPos.z) + _direction.z * 2.0f;
}


/*
 * Position assumed to be relative to the chunk calling the initial command. The calling chunk determines if the
 * position provided is within 0 <= blockPos < ChunkSize, in each .xyz position. If not, it obtains the adjacent chunk
 * in the given direction where the position is outside of the limits.
 *
 * Returns a correct chunk for the _blockPos and updates _blockPos to be relative to the returned chunk
 */

Chunk* Chunk::GetChunkAtPosition(glm::vec3& _blockPos, int _depth) const {
    if (_depth > 1) return nullptr;
    else _depth++;

    // TOP, BOTTOM, FRONT, FRONTLEFT, LEFT, BACKLEFT, BACK, BACKRIGHT, RIGHT, FRONTRIGHT
    if (_blockPos.x < 0) {
        if (adjacentChunks[2] != nullptr) {
            _blockPos.x += chunkSize;
            return adjacentChunks[2]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.x >= chunkSize) {
        if (adjacentChunks[6] != nullptr) {
            _blockPos.x -= chunkSize;
            return adjacentChunks[6]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.y < 0) {
        if (adjacentChunks[1] != nullptr) {
            _blockPos.y += chunkSize;
            return adjacentChunks[1]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.y >= chunkSize) {
        if (adjacentChunks[0] != nullptr) {
            _blockPos.y -= chunkSize;
            return adjacentChunks[0]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.z < 0) {
        if (adjacentChunks[4] != nullptr) {
            _blockPos.z += chunkSize;
            return adjacentChunks[4]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.z >= chunkSize) {
        if (adjacentChunks[8] != nullptr) {
            _blockPos.z -= chunkSize;
            return adjacentChunks[8]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }

    // _blockPos is within this chunk, return pointer to this chunk
    return const_cast<Chunk *>(this);
}