//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_WORLD_H
#define UNTITLED7_WORLD_H

#include <memory>

#include "../Blocks/Cube.h"
#include "../Player/Camera.h"
#include "../Textures/TextureData.h"

class World {
    private:
        TextureData* texture {};
        std::unique_ptr<Cube> skybox;

        // Sky decos: clouds, sun, moon, stars, night, etc

    public:
        World();
        ~World();

        // Display
        void Display();

        // Skybox
        void SetSkyboxProperties(const Camera* camera);
        void SetSkyboxPosition(glm::vec3 _position);
        void SetSkyboxTexture(TextureData* _texture);
};

#endif //UNTITLED7_WORLD_H
