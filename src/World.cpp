//
// Created by cew05 on 10/07/2024.
//

#include "World.h"

World::World() {
    // Create skybox
    skybox = std::make_unique<Cube>();

    // Create skybox texture
    texture = new Texture("../resources/skybox16x.png");
    texture->SetTextureSheetGrid({16, 16});
    skybox->SetTexture(texture, {0,0});

}

World::~World() = default;

void World::Display() {
    texture->EnableTexture();
    skybox->Display();
    texture->DisableTexture();
}

void World::SetSkyboxProperties(const Camera *camera) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = camera->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second-1, 2.0);

    // Create Skybox
    skybox->SetDimensions({float(sqrt(maxSqrd/3)), float(sqrt(maxSqrd/3)), float(sqrt(maxSqrd/3))});
    skybox->SetPositionCentre(camera->GetPosition());
}

void World::SetSkyboxPosition(glm::vec3 _position) {
    skybox->SetPositionCentre(_position);
}

void World::SetSkyboxTexture(Texture *_texture) {
    texture = _texture;
}