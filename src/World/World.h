//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_WORLD_H
#define UNTITLED7_WORLD_H

#include <memory>

#include "../BlockModels/Block.h"
#include "../Player/Camera.h"
#include "../Textures/TextureData.h"
#include "Chunk.h"

static const int worldSize = 8; // n chunks
static const int worldHeight = 4; // from 0 to n chunks high
static const int worldArea = worldSize * worldSize;
static const int worldVolume = worldArea * worldHeight;

namespace WorldDataTypes {
    typedef std::array<std::array<std::array<std::unique_ptr<Chunk>, worldSize>, worldHeight>, worldSize> chunkArray;
    typedef std::array<std::array<std::array<int, worldSize>, worldHeight>, worldSize> biomeMap;
}

class World {
    private:
        Transformation skyboxTransformation;
        std::unique_ptr<Block> skybox;

        // Sky decos: clouds, sun, moon, stars, night, etc

        WorldDataTypes::chunkArray worldChunks {};
        WorldDataTypes::biomeMap biomeMap {};

    public:
        World();
        ~World();

        // Display
        void Display();
        void CheckCulling(const Camera& _camera);

        // Skybox
        void SetSkyboxProperties(const Camera* camera);
        void SetSkyboxPosition(glm::vec3 _position);

        // Generation
        void GenerateWorld();
        void GenerateTerrain();

        // Getters
        [[nodiscard]] Chunk* GetChunkAtPosition(glm::vec3 _position) const;
};

inline std::unique_ptr<World> world {};

#endif //UNTITLED7_WORLD_H
