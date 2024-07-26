//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_CHUNK_H
#define UNTITLED7_CHUNK_H

#include <vector>
#include <memory>

#include "../Blocks/NaturalBlocks.h"
#include "../Blocks/ModelTransformations.h"

static const int chunkSize = 16; // must be power of 2 for subchunk division
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkSize;

// Used to construct a tree of chunks to manage object culling and display
// Either the subchunk vector is populated with further, smaller subchunks, or the Cube object is instantiated
// A SubChunk with a valid cube object is considered to be a terminating point of the subchunk tree.

class Chunk;

class ChunkNode {
    protected:
        // Tree objects
        Chunk* rootChunk {};
        std::vector<std::unique_ptr<ChunkNode>> subNodes {};
        BlockData nodeBlockData {};

        // SubChunk positioning + scale
        std::unique_ptr<Transformation> transformation = std::make_unique<Transformation>();
        glm::vec3 position{0.0f, 0.0f, 0.0f}; // Node position from minimum corner
        glm::vec3 scale{1.0f, 1.0f, 1.0f};    // x by y by z blocks within chunk

        // Display of contents
        bool isSingleType = true;
        bool isCulled = false;

    public:
        ChunkNode(BlockData _nodeBlockData, glm::vec3  _position, Chunk* _root);
        ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root);
        ~ChunkNode();

        void Display();
        void CheckCulling(const Camera& _camera);

        ChunkNode CreateNodeTree(std::array<std::array<std::array<ChunkNode*, chunkSize>, chunkSize>, chunkSize> _blockNodes);
};








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
        std::vector<std::unique_ptr<Block>> uniqueBlocks {};
        std::unique_ptr<ChunkNode> rootNode {};

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);

        // Display and Culling
        void Display();
        void CheckCulling(const Camera& _camera);
        Block* GetBlockFromData(BlockData _data);

        // Chunk Generation
        std::array<int, chunkArea> CreateHeightMap();
        std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> CreateTerrain();
        void CreateNodeTree(std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> _chunkNodes);

        // temp testing
        void MoveChunk(glm::vec3 move);
};


#endif //UNTITLED7_CHUNK_H
