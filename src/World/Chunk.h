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
        std::unique_ptr<Cube> cube {};

        // Is the subchunk within the view frustrum
        std::unique_ptr<BoxBounds> bounds {};
        bool isCulled = false;

        // Transformation matrix for the subchunk
        std::unique_ptr<Transformation> transformation = std::make_unique<Transformation>();;

    public:
        SubChunk();
        explicit SubChunk(Cube* _cube);
        explicit SubChunk(std::vector<std::unique_ptr<SubChunk>> _subChunks);
        ~SubChunk();

        // Updating the model matrix
        void UpdateModelMatrix();
        void UpdateModelMatrix(const glm::mat4& _parentTransformationMatrix);

        // SubChunk Tree Culling and Display
        void CreateCullingBounds();
        void CheckCulling(const Camera& _camera);
        void Display();
};



class Chunk : public SubChunk {
    private:
        static const int chunkSize = 2; // must be power of 2 for subchunk division
        static const int chunkArea = chunkSize * chunkSize;
        static const int chunkVolume = chunkArea * chunkSize;

        glm::vec3 chunkOrigin {0.0f, 0.0f, 0.0f};

    public:
        explicit Chunk(const glm::vec3& _chunkPosition);

        void CreateHeightMap();
        void CreateTerrain();
        void CreateSubchunks(const std::vector<Cube*>& _cubeContainer);

        void CreateCullingBounds();
};


#endif //UNTITLED7_CHUNK_H
