//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_WORLD_H
#define UNTITLED7_WORLD_H

#include <memory>

#include "../BlockModels/Block.h"
#include "../Player/Player.h"
#include "../Textures/TextureData.h"

#include "Chunk.h"
#include "Biome.h"

class Player;

namespace WorldDataTypes {
    typedef std::array<std::array<std::unique_ptr<Chunk>, 2000>, 2000> chunkArray;
    typedef std::array<std::unique_ptr<Biome>, worldArea> biomeMap;
}

class World {
    private:
        Transformation skyboxTransformation;
        std::unique_ptr<Block> skybox;

        // Sky decos: clouds, sun, moon, stars, night, etc
        // ...

        // World Generation
        WorldDataTypes::chunkArray worldChunks {};
        std::vector<std::unique_ptr<Biome>> uniqueBiomes {};
        WorldDataTypes::biomeMap biomeMap {};

        int nChunks {};
        int displayingChunks {};

        glm::ivec2 loadingChunk {1000, 1000}; // inits to origin

    public:
        World();
        ~World();

        // Display
        void Display();
        void CheckCulling(const Camera& _camera);

        // Skybox and Decoratives
        void SetSkyboxProperties(const Player& player);
        void SetSkyboxPosition(glm::vec3 _position);

        // Generation
        void GenerateWorld();
        static float GenerateBlockHeight(glm::vec2 _blockPos);
        static float GenerateBlockHeat(glm::vec3 _blockPos);
        static float GenerateBlockVegetation(glm::vec3 _blockPos, float _heat);
        static ChunkData GenerateChunkData(glm::vec2 _chunkPosition);
        Biome* GenerateBiome(BIOMEID _biomeID);
        void GenerateTerrain(glm::vec3 _loadOrigin);

        // Loading / Unloading chunks around the players centre chunk
        void LoadPlayerChunks(const Chunk* _playerChunk);

        // Getters
        [[nodiscard]] Chunk* GetChunkAtPosition(glm::vec3 _blockPos) const;
        [[nodiscard]] Chunk* GetChunkAtChunkPosition(glm::vec3 _chunkPos) const;
        [[nodiscard]] Chunk* GetChunkFromLoadPosition(glm::vec3 _chunkPos) const;
        [[nodiscard]] Chunk* GetWorldCentreChunk() const { return worldChunks[worldSize/2][worldSize/2].get(); }
        [[nodiscard]] Biome* GetBiome(BIOMEID _biomeID);
};

inline std::unique_ptr<World> world {};

#endif //UNTITLED7_WORLD_H
