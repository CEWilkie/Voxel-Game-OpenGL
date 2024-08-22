//
// Created by cew05 on 11/07/2024.
//

#include "Chunk.h"

#include <glm/gtc/noise.hpp>
#include <memory>
#include <utility>

#include "../Blocks/CreateBlock.h"
#include "../GlobalStates.h"
#include "LoadStructure.h"

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
 * Updates the mesh of a given block. This block must belong to the unique blocks of the chunk, otherwise the blockMesh
 * will not be obtained, even if it is the same blockType being updated.
 */

void Chunk::UpdateBlockMesh(Block* _meshBlock) {
    MaterialMesh* blockMesh = GetMeshFromBlock(_meshBlock);
    if (blockMesh == nullptr || blockMesh->GetBlock()->GetBlockType().blockID == AIR) return;

    blockMesh->ResetVerticies();

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            for (int y = 0; y < chunkHeight; y++) {
                ChunkDataTypes::ChunkBlock block = GetBlockAtPosition({x,y,z}, 0);

                if (BlockType::Compare(block.first->GetBlockType(), _meshBlock->GetBlockType())) {
                    std::vector<Vertex> verticies = block.first->GetFaceVerticies(GetShowingFaces({x,y,z}), block.second);
                    blockMesh->AddVerticies(verticies, {x,y,z});
                }
            }
        }
    }

    needsMeshUpdates = false;
    unboundMeshChanges = true;
}

/*
 * Goes through all positions within the chunk and adds the visible verticies of blocks to their corresponding meshes.
 * Can affect all meshes except any potential air mesh.
 * Will only act upon meshes where "oldMesh" is true
 */

void Chunk::CreateChunkMeshes() {
    for (auto& blockMesh : blockMeshes) {
        if (blockMesh->IsOld()) {
            blockMesh->ResetVerticies();
        }
    }

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            for (int y = 0; y < chunkHeight; y++) {
                ChunkDataTypes::ChunkBlock block = GetBlockAtPosition({x,y,z}, 0);
                MaterialMesh* blockMesh = GetMeshFromBlock(block.first);
                if (blockMesh == nullptr || blockMesh->GetBlock()->GetBlockType().blockID == AIR || !blockMesh->IsOld())
                    continue;

                // Add verticies to mesh
                std::vector<Vertex> verticies = block.first->GetFaceVerticies(GetShowingFaces({x,y,z}), block.second);
                blockMesh->AddVerticies(verticies, {x,y,z});
            }
        }
    }

    for (auto& blockMesh : blockMeshes) {
        if (blockMesh->IsOld()) {
            blockMesh->MarkReadyToBind();
        }
    }

    needsMeshUpdates = false;
    unboundMeshChanges = true;
}



void Chunk::MarkForMeshUpdates() {
    needsMeshUpdates = true;
}


void Chunk::BindChunkMeshes() {
    for (auto& blockMesh : blockMeshes) {
        if (blockMesh->ReadyToBind()) {
            blockMesh->BindMesh();
        }
    }

    unboundMeshChanges = false;
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

    Block* checkingBlock = GetBlockAtPosition(_blockPos, 0).first;
    if (checkingBlock == nullptr) return faces; // Could not find block
    if (BlockType::Compare(checkingBlock->GetBlockType(), {AIR, 0})) return faces; // Air block

    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockAtPosition(_blockPos + positionOffsets[i], 0).first;
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
    Block* checkingBlock = GetBlockAtPosition(_blockPos, 0).first;
    if (checkingBlock == nullptr) return {}; // Could not find block

    // Check for non-transparent block on each face (or non-same transparent block for a transparent block)
    for (int i = 0; i < faces.size(); i++) {
        Block* blockAtFace = GetBlockAtPosition(_blockPos + positionOffsets[i], 0).first;
        if (blockAtFace == nullptr) { // could not obtain block data
//            showingFaces.push_back(faces[i]);
            continue;
        }

        // transparency 1 blocks only show when there is air adjacent
        if (checkingBlock->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 1) {
            // Is not air
            if (!BlockType::Compare(blockAtFace->GetBlockType(), {AIR, 0})) {
                continue;
            }
        }

        // transparency 2 blocks hide back, left or bottom faces which are obscured by non transparency 1 blocks
        if (checkingBlock->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) == 2) {
            if (blockAtFace->GetAttributeValue(BLOCKATTRIBUTE::TRANSPARENT) != 1) {
                if (faces[i] == BACK || faces[i] == LEFT || faces[i] == BOTTOM) continue;
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
    // Populate the terrain array with blocks and update meshes
    CreateTerrain();

    // Generate any structures that appear

    generated = true;
}



/*
 * Generates the chunk terrain and the meshes of the chunk
 */

void Chunk::CreateTerrain() {
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            // Fetch height
            auto hmTopLevel = chunkData.heightMap[x + z * chunkSize];

            for (int y = 0; y < chunkHeight; y++) {
                glm::vec3 blockPos = glm::vec3(x, y, z) + (chunkPosition * (float)chunkSize);
                BlockType newBlockData = chunkData.biome->GetBlockType(hmTopLevel, blockPos.y);
                Block* block = GetBlockFromData(newBlockData);

                // If a block has already been generated for this position and has higher gen priority than the current
                // block attempting to generate, then ignore new gen attempt. Equivalent gen priority sees newest
                // generation overwrite
                if (terrain[x][y][z].first != nullptr) {
                    int generatedPriority = terrain[x][y][z].first->GetAttributeValue(BLOCKATTRIBUTE::GENERATIONPRIORITY);
                    int generatingPriority = block->GetAttributeValue(BLOCKATTRIBUTE::GENERATIONPRIORITY);

                    if (generatedPriority > generatingPriority) continue;
                }

                // Set block and make a new unique instance if required
                SetChunkBlockAtPosition({x,y,z}, newBlockData);

                // give block a random rotation and facing direction
                terrain[x][y][z].second.topFaceDirection = block->GetRandomTopFaceDirection();
                terrain[x][y][z].second.halfRightRotations = block->GetRandomRotation();

            }

            // Now create vegetation provided chunk contains hmToplevel
            hmTopLevel -= chunkPosition.y * chunkSize;
            if (hmTopLevel < 0 || hmTopLevel >= chunkHeight) continue;

            CreateVegitation({x,hmTopLevel,z});
        }
    }

    MarkForMeshUpdates();
}



/*
 *
 */

void Chunk::CreateVegitation(glm::vec3 _blockPos) {
    if (structureLoader == nullptr) return;

    float plantDensity = chunkData.plantMap[(int)_blockPos.x + (int)_blockPos.z * chunkSize];

    Block* block = GetBlockAtPosition(_blockPos, 0).first;
    glm::vec3 plantPos = _blockPos + dirTop;

    if (plantDensity > 1 && block->GetBlockType().blockID == GRASS) {
        structureLoader->StartLoadingStructure(STRUCTURE::TEST);

        int maxB = rand() % 5 + 0;
        for (int b = 0; b < maxB; b++) {
            SetBlockAtPosition(plantPos + glm::vec3(0,b,0), 0,
                               {WOOD, 0});
        }

        plantPos.y += (float)maxB;

        while (structureLoader->LoadedStructure() != STRUCTURE::NONE) {
            StructBlockData blockData = structureLoader->GetStructureBlock();
            Block* blockAtPosition = GetBlockAtPosition(blockData.blockPos + plantPos, 0).first;
            Block* loadingBlock = GetBlockFromData(blockData.blockType);

            // Ensure vegetation can overwrite any current blocks in that position
            if (blockAtPosition != nullptr) {
                int generatedPriority = blockAtPosition->GetAttributeValue(BLOCKATTRIBUTE::GENERATIONPRIORITY);
                int generatingPriority = loadingBlock->GetAttributeValue(BLOCKATTRIBUTE::GENERATIONPRIORITY);

                if (generatedPriority > generatingPriority) continue;
            }

            SetBlockAtPosition(blockData.blockPos + plantPos, 0, blockData.blockType);
        }
    }
    else if (plantDensity < 0.2 && block->GetBlockType().blockID == GRASS) {
        SetBlockAtPosition(plantPos, 0, {LEAVES, 0});
    }


}

/*
 * Set the adjacent chunks in each direction to this chunk
 */

void Chunk::SetAdjacentChunks(const std::array<Chunk*, 8> &_chunks) {
    // -x, -x-z, -z, -z+x, +x, +x+z, +z, +z-x
    adjacentChunks = _chunks;
}



/*
 * Set the block at the given block position to air, and update the meshes of the broken block, and surrounding blocks
 */

void Chunk::BreakBlockAtPosition(glm::vec3 _blockPos) {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, 0);
    if (blockChunk == nullptr) return;

    // Fetch block being broken, if this is somehow null theres probably bigger issues at play so just do nothing
    Block* block = blockChunk->GetBlockAtPosition(_blockPos, 0).first;
    if (block == nullptr) return;

    // Set the block being broken to AIR 0
    blockChunk->SetBlockAtPosition(_blockPos, 0, {AIR, 0});
    blockChunk->MarkForMeshUpdates();

    // update blocks mesh
    MaterialMesh* blockMesh = blockChunk->GetMeshFromBlock(block);
    if (blockMesh != nullptr) blockMesh->MarkOld();

    // Update the meshes of each block adjacent to the block just broken
    std::array<glm::vec3, 7> blockPositions {_blockPos + dirTop, _blockPos + dirBottom, _blockPos + dirLeft,
                                   _blockPos + dirRight, _blockPos + dirFront, _blockPos + dirBack};

    for (auto& blockPosition : blockPositions) {
        Chunk* chunkAtPosition = blockChunk->GetChunkAtPosition(blockPosition, 0);
        if (chunkAtPosition != nullptr) {
            chunkAtPosition->MarkForMeshUpdates();

            Block* chunkBlock = chunkAtPosition->GetBlockAtPosition(blockPosition, 0).first;
            MaterialMesh* chunkMesh = chunkAtPosition->GetMeshFromBlock(chunkBlock);
            if (chunkMesh != nullptr) chunkMesh->MarkOld();
        }
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

    blockChunk->SetChunkBlockAtPosition(_blockPos, _blockType);
    blockChunk->MarkForMeshUpdates();

    // directions of adjacent blocks
    std::array<glm::vec3, 7> blockPositions {_blockPos + dirTop, _blockPos + dirBottom, _blockPos + dirLeft,
                                             _blockPos + dirRight, _blockPos + dirFront, _blockPos + dirBack};

    // Mark block's mesh for recreation and add block mesh's chunk to list
    MaterialMesh* blockMesh = blockChunk->GetMeshFromBlock(block);
    if (blockMesh != nullptr) blockMesh->MarkOld();

    for (auto& blockPosition : blockPositions) {
        Chunk* chunkAtPosition = blockChunk->GetChunkAtPosition(blockPosition, 0);
        if (chunkAtPosition != nullptr) {
            chunkAtPosition->MarkForMeshUpdates();

            Block* chunkBlock = chunkAtPosition->GetBlockAtPosition(blockPosition, 0).first;
            MaterialMesh* chunkMesh = chunkAtPosition->GetMeshFromBlock(chunkBlock);
            if (chunkMesh != nullptr) chunkMesh->MarkOld();
        }
    }
}



/*
 * Sets the block at given position with the given type. blockPos is assumed to have values within 0 - 15
 */

void Chunk::SetChunkBlockAtPosition(const glm::vec3 &_blockPos, const BlockType& _blockType) {
    terrain[(int)_blockPos.x][(int)_blockPos.y][(int)_blockPos.z].first = GetBlockFromData(_blockType);

    // Check if material is new to the chunk
    if (!std::any_of(uniqueBlocks.begin(), uniqueBlocks.end(),
                     [&](std::pair<std::unique_ptr<Block>, int> &uniqueBlock) {
        // Block is unique
        if (!BlockType::Compare(uniqueBlock.first->GetBlockType(), _blockType)) return false;

        // Already exists, increment count
        uniqueBlock.second += 1;
        return true;
    })) {
        // create a new block of the specified type, and create mesh for block
        uniqueBlocks.emplace_back(CreateBlock(_blockType), 1);
    }
}

/*
 * Obtains the chunk that the provided position is within, and then sets the block in that chunk to the specified type.
 * Blockpos is also updated to be relative to the correct chunk it resides within. (eg x = 16 -> x = 0 of adj chunk).
 */

void Chunk::SetBlockAtPosition(glm::vec3 _blockPos, int _depth, const BlockType& _blockType) {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, _depth);
    if (blockChunk == nullptr) return;

    blockChunk->SetChunkBlockAtPosition(_blockPos, _blockType);
}



/*
 * Fetches block at position. Assumes provided position values are within 0 - 15.
 */

ChunkDataTypes::ChunkBlock Chunk::GetChunkBlockAtPosition(const glm::vec3 &_blockPos) {
    return terrain[(int)_blockPos.x][(int)_blockPos.y][(int)_blockPos.z];
}

/*
 * Obtains the chunk that the provided position is within, and then returns the block in the chunk at that position
 */

ChunkDataTypes::ChunkBlock Chunk::GetBlockAtPosition(glm::vec3 _blockPos, int _depth) const {
    Chunk* blockChunk = GetChunkAtPosition(_blockPos, _depth);
    if (blockChunk == nullptr) return {};

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
    Block* playerBlock = GetBlockAtPosition(_blockPos + dirTop, 0).first;
    if (playerBlock != nullptr && playerBlock->GetAttributeValue(BLOCKATTRIBUTE::ENTITYCOLLISIONSOLID) != 0) {
        return GetTopLevelAtPosition(_blockPos + dirTop, _radius);
    }

    float topLevel = -20;

    // if position y is 0.8 or above, round to ciel
    float y = (_blockPos.y - floorf(_blockPos.y) >= 0.8f) ? roundf(_blockPos.y) : floorf(_blockPos.y);

    // to 0.01 precision, convert to int to *maybe* stop some imprecision issues with looping through with floats
    for (int x = int(100.0 * (_blockPos.x - _radius)); x <= int(100.0 * (_blockPos.x + _radius)); x += int(100.0 * _radius)) {
        for (int z = int(100.0 * (_blockPos.z - _radius)); z <= int(100.0 * (_blockPos.z + _radius)); z += int(100.0 * _radius)) {
            // Convert back to float position of block relative to chunk
            glm::vec3 position{x/100.0, y, z/100.0};
            Block* block = GetBlockAtPosition(position, 0).first;

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
    ChunkDataTypes::ChunkBlock block = GetBlockAtPosition(_blockPos + _direction, 0);
    if (block.first == nullptr || block.first->GetBlockType().blockID == AIR || block.first->GetAttributeValue(BLOCKATTRIBUTE::LIQUID) > 0) {
        if (_direction.x != 0) return floorf(_blockPos.x) + _direction.x * 2.0f;
        if (_direction.y != 0) return floorf(_blockPos.y) + _direction.y * 2.0f;
        if (_direction.z != 0) return floorf(_blockPos.z) + _direction.z * 2.0f;
    }

    BLOCKFACE face {};
    std::vector<Vertex> faceVerticies {};
    float minZ {0}, maxZ {0};
    float minX {0}, maxX {0};

    if (_direction == dirFront) face = BACK;
    if (_direction == dirBack) face = FRONT;
    if (_direction == dirLeft) face = RIGHT;
    if (_direction == dirRight) face = LEFT;

    // Get min and max face verticies for x and z position
    faceVerticies = block.first->GetFaceVerticies({face}, block.second);
    for (auto& vertex : faceVerticies) {
        if (vertex.position.z + glm::ceil(_blockPos.z) + _direction.z < minZ)
            minZ = vertex.position.z + glm::ceil(_blockPos.z) + _direction.z;
        if (vertex.position.z + glm::floor(_blockPos.z) + _direction.z > maxZ)
            maxZ = vertex.position.z + glm::floor(_blockPos.z) + _direction.z;

        if (vertex.position.x + glm::ceil(_blockPos.x) + _direction.x < minX)
            minX = vertex.position.x + glm::ceil(_blockPos.x) + _direction.x;
        if (vertex.position.x + glm::ceil(_blockPos.x) + _direction.x > maxX)
            maxX = vertex.position.x + glm::floor(_blockPos.x) + _direction.x;
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

    // FRONT, FRONTLEFT, LEFT, BACKLEFT, BACK, BACKRIGHT, RIGHT, FRONTRIGHT
    if (_blockPos.x < 0) {
        if (adjacentChunks[0] != nullptr) {
            _blockPos.x += chunkSize;
            return adjacentChunks[0]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.x >= chunkSize) {
        if (adjacentChunks[4] != nullptr) {
            _blockPos.x -= chunkSize;
            return adjacentChunks[4]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.y < 0 || _blockPos.y >= chunkHeight) {
        return nullptr;
    }
    if (_blockPos.z < 0) {
        if (adjacentChunks[2] != nullptr) {
            _blockPos.z += chunkSize;
            return adjacentChunks[2]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }
    if (_blockPos.z >= chunkSize) {
        if (adjacentChunks[6] != nullptr) {
            _blockPos.z -= chunkSize;
            return adjacentChunks[6]->GetChunkAtPosition(_blockPos, _depth);
        }
        else
            return nullptr;
    }

    // _blockPos is within this chunk, return pointer to this chunk
    return const_cast<Chunk *>(this);
}