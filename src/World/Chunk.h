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

        // Positioning
        float scale = 1;
        glm::vec3 position {0,0,0};

        // Merge nodes
        bool isSingleType = true;

    public:
        ChunkNode(Block* _nodeBlock, glm::vec3  _blockPos, Chunk* _root);
        ChunkNode(std::vector<std::unique_ptr<ChunkNode>> _subNodes, Chunk* _root);
        ~ChunkNode();

        // Node material mesh creation
        void UpdateMaterialMesh(MaterialMesh* _mesh);
};







inline int nChunksCreated;
inline Uint64 chunkAvgTicksTaken = 0;
inline Uint64 chunkSumTicksTaken = 0;

inline int nMeshesCreated;
inline Uint64 meshAvgTicksTaken = 0;
inline Uint64 meshSumTicksTaken = 0;

namespace ChunkDataTypes {
    typedef std::array<std::array<std::array<std::unique_ptr<ChunkNode>, chunkSize>, chunkSize>, chunkSize> nodeArray;
    typedef std::array<std::array<std::array<BlockType, chunkSize>, chunkSize>, chunkSize> terrainArray;
}


/*
 * Parent node to ChunkNodes
 */

class Chunk {
    private:
        // Chunk Display
        std::vector<std::unique_ptr<MaterialMesh>> blockMeshes {};

        // Chunk Culling and positioning
        std::unique_ptr<BoxBounds> boxBounds {};
        std::unique_ptr<Transformation> chunkTransformation = std::make_unique<Transformation>();
        glm::vec3 chunkPosition {0,0,0};
        bool inCamera = true;

        // Chunk tree
        std::unique_ptr<ChunkNode> rootNode {};
        std::vector<Chunk*> adjacentChunks {};

        // Block Data
        std::vector<std::pair<std::unique_ptr<Block>, int>> uniqueBlocks {}; // block, count
        ChunkDataTypes::terrainArray terrain {};

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);
        ~Chunk();

        // Display
        void DisplaySolid();
        void DisplayTransparent();

        // Object Culling
        void CheckCulling(const Camera& _camera);
        [[nodiscard]] std::vector<BLOCKFACE> GetHiddenFaces(glm::vec3 _position) const;
        [[nodiscard]] std::vector<BLOCKFACE> GetShowingFaces(glm::vec3 _position) const;

        // Chunk Generation
        std::array<int, chunkArea> CreateHeightMap();
        ChunkDataTypes::nodeArray CreateTerrain();
        void CreateNodeTree(ChunkDataTypes::nodeArray _chunkNodes);
        void SetAdjacentChunks(const std::vector<Chunk*>& _chunks);

        // Getters
        [[nodiscard]] BlockType GetBlockDataAtPosition(glm::vec3 _position) const;
        [[nodiscard]] Block* GetBlockFromData(BlockType _blockData) const;
        [[nodiscard]] glm::vec3 GetPosition() const { return chunkPosition; }
};


#endif //UNTITLED7_CHUNK_H
