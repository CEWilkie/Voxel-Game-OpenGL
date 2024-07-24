//
// Created by cew05 on 10/07/2024.
//

#include "World.h"

World::World() {
    // Create skybox
    skybox = std::make_unique<Cube>();

    // Create skybox texture
    skybox->SetTexture(TEXTURESHEET::WORLD, {1,1});

}

World::~World() = default;

void World::Display() {
    glEnable(GL_DEPTH_TEST);

    // First draw in the skybox and decorations
    skybox->Display();

    // Now draw the world terrain / objecst
    glEnable(GL_CULL_FACE);

    for (const auto& chunk : worldChunks) chunk->Display();

    glDisable(GL_CULL_FACE);


    worldChunks[0]->MoveChunk({0.0f, 0.001f, 0.0f});
}

void World::CheckCulling(const Camera &_camera) {
    for (auto& chunk : worldChunks) chunk->CheckCulling(_camera);
}

void World::SetSkyboxProperties(const Camera *camera) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = camera->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second-1, 2.0);

    // Set skybox scale
    skybox->SetScale({float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3))});
    skybox->SetPositionCentre(camera->GetPosition());
    skybox->UpdateModelMatrix();
}

void World::SetSkyboxPosition(glm::vec3 _position) {
    skybox->SetPositionCentre(_position);
    skybox->UpdateModelMatrix();
}


/*
 * WORLD GENERATION
 */

void World::GenerateWorld() {
    // First generate the world terrain
    GenerateTerrain();
}

void World::GenerateTerrain() {
    for (int chunkX = -worldSize/2; chunkX < worldSize/2; chunkX++) {
        for (int chunkZ = -worldSize/2; chunkZ < worldSize/2; chunkZ++) {
            glm::vec3 chunkPos{chunkX, 0, chunkZ};
            worldChunks.push_back(std::make_unique<Chunk>(chunkPos));
        }
    }
}
