//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_CHUNK_H
#define UNTITLED7_CHUNK_H

#include <vector>
#include <memory>

#include "../Blocks/NaturalBlocks.h"
#include "../BlockModels/ModelTransformations.h"
#include "../BlockModels/ChunkMesh.h"

static const int chunkSize = 16; // must be power of 2 for subchunk division ie 2, 4, 8, 16 | 32, 64, 128, ... ( too big)
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkSize;

class Chunk;

// Used to construct a tree of chunks to manage object culling and display
// Either the subchunk vector is populated with further, smaller subchunks, or the Cube object is instantiated
// A SubChunk with a valid cube object is considered to be a terminating point of the subchunk tree.

class ChunkNode {
    protected:
        // Tree objects
        Chunk* rootChunk {};
        std::vector<std::unique_ptr<ChunkNode>> subNodes {};
        Block* nodeBlock {};

        // SubChunk positioning + scale
        std::unique_ptr<Transformation> transformation = std::make_unique<Transformation>();

        // Display of contents
        bool isSingleType = true;
        bool isCulled = false;
        bool visible = false;

    public:
        ChunkNode(Block* _nodeBlock, glm::vec3  _position, Chunk* _root);
        ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root);
        ~ChunkNode();

        void Display();
        void CheckCulling(const Camera& _camera);
};







inline int nChunksCreated;
inline Uint64 averageTicksTaken = 0;
inline Uint64 sumTicksTaken = 0;

/*
 * Parent node to ChunkNodes
 */

class Chunk {
    private:
        glm::vec3 chunkOrigin {0.0f, 0.0f, 0.0f};

        // Transformation matrix for the chunk
        std::unique_ptr<Transformation> transformation {};

        // Culling Bounds
        std::vector<std::unique_ptr<BoxBounds>> boxBounds {};

        // Block Data
        std::vector<std::pair<std::unique_ptr<Block>, int>> uniqueBlocks {}; // block, count
        std::unique_ptr<ChunkNode> rootNode {};

        // Block Data
        std::array<std::array<std::array<std::unique_ptr<Block>, chunkSize>, chunkSize>, chunkSize> terrain {};
        std::unique_ptr<ChunkMesh> chunkMesh = std::make_unique<ChunkMesh>();


        std::vector<Block*> blocks;

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);

        // Display
        void Display();
        Block* GetBlockFromData(BlockData _data);

        // Object Culling / Mesh Creation
        void CheckCulling(const Camera& _camera);
        Block* GetBlockAtPosition(glm::vec3 _position);
        std::vector<BLOCKFACE> CheckFaceCulling(glm::vec3 _position);
        void CreateChunkMesh();

        // Chunk Generation
        std::array<int, chunkArea> CreateHeightMap();
        std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> CreateTerrain();
        void CreateNodeTree(std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> _chunkNodes);

        // temp testing
        void MoveChunk(glm::vec3 move);
};


#endif //UNTITLED7_CHUNK_H
