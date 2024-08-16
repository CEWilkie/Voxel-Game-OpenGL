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
#include "../Player/Camera.h"

#include "WorldGenConsts.h"
#include "Biome.h"

class Chunk;

/*
 * Legacy stuff for octree implementations. Currently not used in favour of a more simple 3d-array approach for
 * chunk blocks. Going to leave the code in the files for the potential of future re-implementation.
 */

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





/*
 * Simple data struct used to hold various maps of the chunk's blocks and biome information.
 */

struct ChunkData {
    Biome* biome {};
    ChunkDataMap heightMap {};
    ChunkDataMap heatMap {};
};

/*
 * Houses a 3D array of blocks, of cubic size 16x16x16 (chunkSize^3). Resonsible for generating blocks from a given set
 * of maps (see ChunkData) and then generating biome-specific structures. Chunk also incorporates pointers to the
 * surrounding 10 adjacent chunks (diagonal top/bottoms not included) which can be used for mesh creation and various
 * block updates / interactions.
 */

class Chunk {
    private:
        // Chunk Culling and Display
        std::unique_ptr<BoxBounds> boxBounds {};
        Transformation cullingTransformation {};
        Transformation displayTransformation {};
        bool inCamera = true;

        // Chunk Terrain and Block Data
        std::vector<std::pair<std::unique_ptr<Block>, int>> uniqueBlocks {};                                            // block, count
        std::vector<std::unique_ptr<MaterialMesh>> blockMeshes {};                                                      // for each unique block
        chunkTerrainArray terrain {};
        bool generated = false;

        // Unique ChunkData and the adjacent Chunk pointers
        ChunkData chunkData;
        glm::vec3 chunkPosition {0,0,0};
        std::array<Chunk*, 10> adjacentChunks {};

        // Private functions for getting/setting blocks which non-chunks shouldn't access
        [[nodiscard]] Block* GetChunkBlockAtPosition(const glm::vec3& _blockPos);
        void SetChunkBlockAtPosition(const glm::vec3& _blockPos, const BlockType& _blockType);

    public:
        Chunk(const glm::vec3& _chunkPosition, ChunkData _chunkData);
        ~Chunk();

        // Chunk Display
        void DisplaySolid();
        void DisplayTransparent();

        // Chunk Block Meshes Creation / Updating
        void CreateBlockMeshes();
        void UpdateMeshAtPosition(glm::vec3 _blockPos);
        void UpdateBlockMesh(Block* _meshBlock);
        void CreateChunkMeshes();
        [[nodiscard]] std::vector<BLOCKFACE> GetHiddenFaces(glm::vec3 _blockPos) const;
        [[nodiscard]] std::vector<BLOCKFACE> GetShowingFaces(glm::vec3 _blockPos) const;
        [[nodiscard]] MaterialMesh* GetMeshFromBlock(Block* _block);

        // Chunk Culling
        void CheckCulling(const Camera& _camera);
        [[nodiscard]] bool ChunkVisible() const { return inCamera; };

        // Chunk Terrain and Structures Generation
        void GenerateChunk();
        void CreateTerrain();
        void SetAdjacentChunks(const std::array<Chunk*, 10>& _chunks);

        // Chunk Block Interaction
        void BreakBlockAtPosition(glm::vec3 _blockPos);
        void PlaceBlockAtPosition(glm::vec3 _blockPos, BlockType _blockType);
        void SetBlockAtPosition(glm::vec3 _blockPos, int _depth, const BlockType& _blockType);
        [[nodiscard]] Block* GetBlockAtPosition(glm::vec3 _blockPos, int _depth) const;

        // Chunk-Entity Collision
        [[nodiscard]] float GetTopLevelAtPosition(glm::vec3 _blockPos, float _radius) const;
        [[nodiscard]] float GetDistanceToBlockFace(glm::vec3 _blockPos, glm::vec3 _direction, float _radius) const;

        //
        [[nodiscard]] Block* GetBlockFromData(const BlockType& _blockType);
        [[nodiscard]] glm::vec3 GetPosition() const { return chunkPosition; }
        [[nodiscard]] Chunk* GetChunkAtPosition(glm::vec3& _blockPos, int _depth) const;
};


#endif //UNTITLED7_CHUNK_H
