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
    typedef std::array<std::array<std::array<std::unique_ptr<Chunk>, worldSize>, worldHeight>, worldSize> chunkArray;
    typedef std::array<std::unique_ptr<Biome>, worldArea> biomeMap;
    typedef std::array<float, worldArea> chunkDataMap;
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

    public:
        World();
        ~World();

        // Display
        void Display();
        void CheckCulling(const Camera& _camera);

        // Skybox
        void SetSkyboxProperties(const Player& player);
        void SetSkyboxPosition(glm::vec3 _position);

        // Generation
        void GenerateWorld();
        float GenerateBlockHeight(glm::vec2 _blockPos);
        ChunkData GenerateChunkData(glm::vec2 _chunkPosition);
        Biome* GenerateBiome(BIOMEID _biomeID);
        void GenerateTerrain();

        // Getters
        [[nodiscard]] Chunk* GetChunkAtPosition(glm::vec3 _position) const;
        [[nodiscard]] Biome* GetBiome(BIOMEID _biomeID);
};

inline std::unique_ptr<World> world {};

#endif //UNTITLED7_WORLD_H
