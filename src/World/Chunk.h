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

class SubChunk {
    protected:
        // Tree object pointers
        std::vector<std::unique_ptr<SubChunk>> subChunks {};
        std::vector<Block*> subCubes {};

        // SubChunk culling
        std::unique_ptr<BoxBounds> bounds {};
        bool isCulled = false;

    public:
        SubChunk();
        explicit SubChunk(const std::vector<Block*>& _subCubes);
        explicit SubChunk(const std::vector<SubChunk*>& _subChunks);
        ~SubChunk();

        // Updating the model matrix
        void UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix);

        // SubChunk Tree Culling and Display
        void CheckCulling(const Camera& _camera);
        void Display();

        [[nodiscard]] BoxBounds GetBounds() const { return *bounds; }
};



class Chunk {
    private:
        static const int chunkSize = 4; // must be power of 2 for subchunk division
        static const int chunkArea = chunkSize * chunkSize;
        static const int chunkVolume = chunkArea * chunkSize;

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

        void CreateHeightMap();
        void CreateTerrain();
        void CreateSubchunks(const std::vector<std::vector<std::vector<Block*>>>& _xzyCubeContainer);

        void UpdateCubeMatricies();
        void MoveChunk(glm::vec3 move);
};


#endif //UNTITLED7_CHUNK_H
