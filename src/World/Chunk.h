//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_CHUNK_H
#define UNTITLED7_CHUNK_H

#include <vector>
#include <memory>

#include "../Blocks/NaturalBlocks.h"
#include "../Blocks/ModelTransformations.h"

// Used to construct a tree of chunks to manage object culling and display
// Either the subchunk vector is populated with further, smaller subchunks, or the Cube object is instantiated
// A SubChunk with a valid cube object is considered to be a terminating point of the subchunk tree.

class ChunkNode {
    protected:
        // Tree object pointers
        std::vector<std::unique_ptr<ChunkNode>> subNodes {};
        std::vector<BlockData> subBlocks {};

        // SubChunk culling
        Transformation* transformation {};
        bool isCulled = false;

        // Display of contents
        bool isSingleType = false;
        int nBlocks = 0;

    public:
        ChunkNode();
        ~ChunkNode();
};






static const int chunkSize = 8; // must be power of 2 for subchunk division
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkSize;

class Chunk {
    private:
        glm::vec3 chunkOrigin {0.0f, 0.0f, 0.0f};

        // Transformation matrix for the chunk
        std::unique_ptr<Transformation> transformation {};

        // Culling Bounds
        std::vector<std::unique_ptr<BoxBounds>> boxBounds {};

        // Block Data
        std::vector<TEXTURESHEET> blockSheets {};
        std::vector<std::unique_ptr<Block>> chunkCubes {};

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);

        void Display();
        void CheckCulling(const Camera& _camera);

        std::array<int, chunkArea> CreateHeightMap();
        void CreateTerrain();
        void CreateSubchunks(const std::vector<std::vector<std::vector<Block*>>>& _xzyCubeContainer);

        void UpdateCubeMatricies();
        void MoveChunk(glm::vec3 move);
};


#endif //UNTITLED7_CHUNK_H
