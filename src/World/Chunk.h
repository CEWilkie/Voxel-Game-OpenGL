//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_CHUNK_H
#define UNTITLED7_CHUNK_H

#include <vector>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "../BlockModels/MaterialMesh.h"
#include "../Player/Camera.h"
#include "WorldGenConsts.h"
#include "Biome.h"

// CHUNK TYPEDEFS
namespace ChunkDataTypes {
    struct ChunkBlock {
        BlockType type {AIR, 0};
        BlockAttributes attributes;
    };

    typedef std::array<std::array<std::array<ChunkBlock, chunkSize>, chunkHeight>, chunkSize> TerrainArray;
    typedef std::array<std::array<std::array<float, chunkSize>, chunkHeight>, chunkSize> DensityArray;
    typedef std::array<float, chunkArea> DataMap;
}

/*
 * Simple data struct used to hold various maps of the chunk's blocks and biome information.
 */

struct ChunkData {
    Biome* biome {};
    ChunkDataTypes::DataMap heightMap {};
    ChunkDataTypes::DataMap heatMap {};
    ChunkDataTypes::DataMap plantMap {};
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
        bool needsMeshUpdates = false;
        bool unboundMeshChanges = false;

        // Chunk Terrain and Block Data
        std::unordered_map<BlockType, std::unique_ptr<Block>> uniqueBlockMap {};
        std::unordered_map<BlockType, std::unique_ptr<MaterialMesh>> uniqueMeshMap {};
        ChunkDataTypes::TerrainArray terrain {};
        bool generated = false;

        // Unique ChunkData and the adjacent Chunk pointers
        ChunkData chunkData;
        glm::vec3 chunkIndex {0, 0, 0};

        // Private functions for getting/setting blocks which non-chunks shouldn't access
        [[nodiscard]] ChunkDataTypes::ChunkBlock GetChunkBlockAtPosition(const glm::vec3& _blockPos);
        void SetChunkBlockAtPosition(const glm::vec3& _blockPos, const BlockType& _blockType);

    public:
        Chunk(const glm::vec3& _chunkPosition, ChunkData _chunkData);
        ~Chunk();

        // Chunk Display
        void DisplaySolid();
        void DisplayTransparent();

        // Chunk Block Meshes Creation / Updating
        void UpdateBlockMesh(Block* _meshBlock);
        void CreateChunkMeshes();
        void MarkForMeshUpdates();
        void BindChunkMeshes();
        [[nodiscard]] bool NeedsMeshUpdates() const { return needsMeshUpdates; }
        [[nodiscard]] bool UnboundMeshChanges() const { return unboundMeshChanges; }
        [[nodiscard]] std::vector<BLOCKFACE> GetHiddenFaces(glm::vec3 _blockPos);
        [[nodiscard]] std::vector<BLOCKFACE> GetShowingFaces(glm::vec3 _blockPos, const Block& _checkingBlock);
        [[nodiscard]] MaterialMesh* GetMeshFromBlock(const BlockType& _blockType);

        // Chunk Culling
        void CheckCulling(const Camera& _camera);
        [[nodiscard]] bool ChunkVisible() const { return inCamera; };

        // Chunk Terrain and Structures Generation
        void GenerateChunk();
        void CreateTerrain();
        void CreateVegitation(glm::vec3 _blockPos);
        [[nodiscard]] bool Generated() const { return generated; }
        [[nodiscard]] bool RegionGenerated() const;

        // Chunk Block Interaction
        void BreakBlockAtPosition(glm::vec3 _blockPos);
        void PlaceBlockAtPosition(glm::vec3 _blockPos, BlockType _blockType);
        void SetBlockAtPosition(glm::vec3 _blockPos, const BlockType& _blockType) const;
        [[nodiscard]] ChunkDataTypes::ChunkBlock GetBlockAtPosition(glm::vec3 _blockPos) const;

        // Chunk-Entity Collision
        [[nodiscard]] float GetTopLevelAtPosition(glm::vec3 _blockPos, float _radius) ;
        [[nodiscard]] float GetDistanceToBlockFace(glm::vec3 _blockPos, glm::vec3 _direction, float _radius) ;

        //
        [[nodiscard]] Block& GetBlockFromData(const BlockType& _blockType);
        [[nodiscard]] glm::vec3 GetIndex() const { return chunkIndex; }
        [[nodiscard]] std::shared_ptr<Chunk> GetChunkAtBlockPos(glm::vec3& _blockPos) const;
};


#endif //UNTITLED7_CHUNK_H
