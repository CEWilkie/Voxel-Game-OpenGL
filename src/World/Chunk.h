//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_CHUNK_H
#define UNTITLED7_CHUNK_H

#include <vector>
#include <memory>

#include "../Blocks/NaturalBlocks.h"
#include "../BlockModels/ModelTransformations.h"
#include "../BlockModels/MaterialMesh.h"

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
        float scale = 1;

        // Display of contents
        std::unique_ptr<MaterialMesh> blockMesh {};
        bool isSingleType = true;
        bool inCamera = true;
        bool visible = false;

    public:
        ChunkNode(Block* _nodeBlock, glm::vec3  _blockPos, Chunk* _root);
        ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root);
        ~ChunkNode();

        // Node Display
        void Display();

        // Node material mesh creation
        void CreateMaterialMesh();

        // Node Culling
        void CheckCulling(const Camera& _camera);
        void CheckNodeCulled();
};







inline int nChunksCreated;
inline Uint64 averageTicksTaken = 0;
inline Uint64 sumTicksTaken = 0;

typedef std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> nodeArray;
typedef std::array<std::array<std::array<BlockType, chunkSize>, chunkSize>, chunkSize> terrainArray;

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
        terrainArray terrain {};
        std::vector<MaterialMesh> chunkMesh{};

        std::vector<Block*> blocks;

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);

        // Display
        void Display();

        // Object Culling
        void CheckCulling(const Camera& _camera);
        std::vector<BLOCKFACE> GetHiddenFaces(glm::vec3 _position) const;
        std::vector<BLOCKFACE> GetShowingFaces(glm::vec3 _position) const;

        // Chunk Generation
        std::array<int, chunkArea> CreateHeightMap();
        nodeArray CreateTerrain();
        void CreateNodeTree(nodeArray _chunkNodes);

        // Getters
        [[nodiscard]] BlockType GetBlockDataAtPosition(glm::vec3 _position) const;
        [[nodiscard]] Block* GetBlockFromData(BlockType _blockData) const;
        [[nodiscard]] glm::vec3 GetPosition() const { return transformation->GetLocalPosition(); }
};


#endif //UNTITLED7_CHUNK_H
