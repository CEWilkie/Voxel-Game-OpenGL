//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_WORLD_H
#define UNTITLED7_WORLD_H

#include <memory>

#include "../Blocks/Block.h"
#include "../Player/Camera.h"
#include "../Textures/TextureData.h"
#include "Chunk.h"

class World {
    private:
        std::unique_ptr<Block> skybox;

        // Sky decos: clouds, sun, moon, stars, night, etc

        // Generation
        static const int worldSize = 2; // n chunks
        static const int worldArea = worldSize * worldSize;

        std::vector<std::unique_ptr<Chunk>> worldChunks {};

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

};

inline std::unique_ptr<World> world {};

#endif //UNTITLED7_WORLD_H
