//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_WORLD_H
#define UNTITLED7_WORLD_H

#include <memory>
#include <thread>

#include "../BlockModels/Block.h"
#include "../Player/Player.h"
#include "../Textures/TextureData.h"

#include "Chunk.h"
#include "Biome.h"
#include "ChunkThreads.h"

class Player;

enum class THREAD {
        CHUNKBUILDING, CHUNKMESHING, CHUNKLIGHTING // ...
};

namespace WorldDataTypes {
    typedef std::array<std::array<std::unique_ptr<Chunk>, 2000>, 2000> chunkArray;
    typedef std::array<std::unique_ptr<Biome>, worldArea> biomeMap;
}

class World {
    private:
        Transformation skyboxTransformation, sunTransformation, moonTransformation;
        std::unique_ptr<Block> skybox;

        // Sky decos: clouds, sun, moon, stars, etc
        std::unique_ptr<Block> sun, moon;
        unsigned int worldTicks = 0;
        unsigned int worldTime = 7*60; // minutes
        unsigned int worldDays = 0;

        // World Generation
        WorldDataTypes::chunkArray worldChunks {};
        std::vector<std::unique_ptr<Biome>> uniqueBiomes {};
        WorldDataTypes::biomeMap biomeMap {};

        int displayingChunks {};

        // Threads
        ChunkThreads chunkBuilderThread;
        ChunkThreads chunkMesherThread;

        glm::ivec2 loadingChunk {0, 0}; // centre

    public:
        World();
        ~World();

        // Display
        void Display() const;
        void CheckCulling(const Camera& _camera);

        // Skybox and Decoratives
        void SetSkyboxProperties(const Player& player);
        void SetSkyboxPosition(glm::vec3 _position);
        void UpdateWorldTime(Uint64 _deltaTicks);

        // Thread Functions
        void CreateChunk(const glm::ivec2& _chunkPos, const glm::vec3& _blockPos);
        void CreateChunkAtIndex(const glm::ivec2& _chunkPos, const ChunkData& _chunkData);

        void GenerateChunk(const glm::ivec2& _chunkPos, const glm::vec3& _blockPos);
        void GenerateChunkMesh(const glm::ivec2& _chunkPos, const glm::vec3& _blockPos);

        // ChunkData Generation functions
        static float GenerateBlockHeight(glm::vec2 _blockPos);
        static float GenerateBlockHeat(glm::vec3 _blockPos);
        static float GenerateBlockVegetation(glm::vec3 _blockPos, float _heat);
        static ChunkData GenerateChunkData(glm::vec2 _chunkPosition);
        Biome* GenerateBiome(BIOMEID _biomeID);


        void SetLoadingOrigin(const glm::vec3& _origin);
        void GenerateLoadedWorld();

        void BindChunks() const;

        // Getters
        [[nodiscard]] Chunk* GetChunkAtPosition(glm::vec3 _blockPos) const;
        [[nodiscard]] Chunk* GetChunkAtIndex(glm::vec3 _chunkPos) const;
        [[nodiscard]] Chunk* GetChunkLoadRelative(glm::vec3 _chunkPos) const;
        [[nodiscard]] Chunk* GetWorldCentreChunk() const { return worldChunks[worldSize/2][worldSize/2].get(); }
        [[nodiscard]] Biome* GetBiome(BIOMEID _biomeID);

        [[nodiscard]] ChunkThreads* GetThread(THREAD _thread) {
            switch (_thread) {
                case THREAD::CHUNKBUILDING:
                    return &chunkBuilderThread;

                case THREAD::CHUNKMESHING:
                    return &chunkMesherThread;

                default:
                    return nullptr;
            }
        }
};

inline std::unique_ptr<World> world {};

#endif //UNTITLED7_WORLD_H
