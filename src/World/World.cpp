//
// Created by cew05 on 10/07/2024.
//

#include "World.h"
#include "../Blocks/CreateBlock.h"

World::World() {
    // Create skybox
    skybox = CreateBlock({BLOCKID::AIR, 1});
}

World::~World() = default;

void World::Display() {
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_NEVER);

    // First draw in the skybox and decorations
    skybox->Display(&skyboxTransformation);

    // Now draw the world terrain / objecst
    glEnable(GL_CULL_FACE);
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                worldChunks[chunkX][chunkY][chunkZ]->Display();
            }
    glDisable(GL_CULL_FACE);
}

void World::CheckCulling(const Camera &_camera) {
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                worldChunks[chunkX][chunkY][chunkZ]->CheckCulling(_camera);
            }



}



/*
 * SKYBOX
 */

void World::SetSkyboxProperties(const Camera *camera) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = camera->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second-1, 2.0);

    // Set skybox scale
    skyboxTransformation.SetScale({float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3)), float(sqrt(maxSqrd / 3))});

    // Set skybox position centred on the player
    SetSkyboxPosition(camera->GetPosition());
}

void World::SetSkyboxPosition(glm::vec3 _position) {
    glm::vec3 originFromCentre{_position - (skyboxTransformation.GetLocalScale() / 2.0f)};
    originFromCentre.y += skyboxTransformation.GetLocalScale().y;
    skyboxTransformation.SetPosition(originFromCentre);

    skyboxTransformation.UpdateModelMatrix();
}


/*
 * WORLD GENERATION
 */

void World::GenerateWorld() {
    // First generate the world terrain
    GenerateTerrain();

    printf("AVG CHUNK CREATION: %llu TICKS TAKEN\n", chunkAvgTicksTaken);
    printf("AVG MESH CREATION: %llu TICKS TAKEN\n", meshAvgTicksTaken);
}

void World::GenerateTerrain() {
    for (int chunkX = 0; chunkX < worldSize; chunkX++)
        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
                glm::vec3 chunkPos{chunkX - worldSize/2, chunkY, chunkZ - worldSize/2};
                worldChunks[chunkX][chunkY][chunkZ] = std::make_unique<Chunk>(chunkPos);
            }

    // Assign neighbouring chunks to created chunks
//    for (int chunkX = 0; chunkX < worldSize; chunkX++)
//        for (int chunkY = 0; chunkY < worldHeight; chunkY++)
//            for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {
//                std::vector<Chunk*> adjacentChunks {};
//
//
//
//
//
//            }
}

