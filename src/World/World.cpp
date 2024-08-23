//
// Created by cew05 on 10/07/2024.
//

#include "glm/gtc/noise.hpp"

#include "World.h"
#include "../Blocks/CreateBlock.h"
#include "CreateBiome.h"
#include "../Window.h"

World::World() {
    // Create skybox, sun and moon
    skybox = CreateBlock({BLOCKID::AIR, 1});
    sun = CreateBlock({AIR, 2});
    moon = CreateBlock({AIR, 3});

    sunTransformation = Transformation();
    sunTransformation.SetScale({15.0f, 15.0f, 15.0f});
    sunTransformation.UpdateModelMatrix();

    moonTransformation = Transformation();
    moonTransformation.SetScale({12.0f, 12.0f, 12.0f});
    moonTransformation.UpdateModelMatrix();

    GLint uLocation;
    uLocation = glGetUniformLocation(window.GetShader(), "worldAmbients.lightingStrength");
    if (uLocation < 0) printf("location not found [worldAmbients.lightingStrength]\n");
    else glUniform1f(uLocation, 1.0f);

    uLocation = glGetUniformLocation(window.GetShader(), "worldAmbients.minFogDistance");
    if (uLocation < 0) printf("location not found [worldAmbients.minFogDistance]\n");
    else glUniform1f(uLocation, (loadRadius - 1) * chunkSize);

    uLocation = glGetUniformLocation(window.GetShader(), "worldAmbients.maxFogDistance");
    if (uLocation < 0) printf("location not found [worldAmbients.maxFogDistance]\n");
    else glUniform1f(uLocation, loadRadius * chunkSize);

    glEnable(GL_DEPTH_TEST);
}

World::~World() {
    ToggleChunkThreads(false);
    chunkBuilder.join();
    chunkMesher.join();
}

void World::Display() const {
    glEnable(GL_BLEND);

    // First draw in the skybox and decorations
    skybox->Display(skyboxTransformation);
    sun->Display(sunTransformation);
    moon->Display(moonTransformation);

    // Draw solid objects
    glEnable(GL_CULL_FACE);
    for (int x = -loadRadius + 1; x < loadRadius; x++) {
        for (int z = -loadRadius + 1; z < loadRadius + 10; z++) {
            Chunk* chunk = GetChunkLoadRelative({x, 0, z});
            if (chunk != nullptr) {
                chunk->DisplaySolid();
            }
        }
    }
    glDisable(GL_CULL_FACE);

    // Draw transparent objects
    for (int x = -loadRadius + 1; x < loadRadius; x++) {
        for (int z = -loadRadius + 1; z < loadRadius + 10; z++) {
            Chunk* chunk = GetChunkLoadRelative({x, 0, z});
            if (chunk != nullptr) chunk->DisplayTransparent();
        }
    }

}

void World::CheckCulling(const Camera &_camera) {
    displayingChunks = 0;

    for (int chunkX = 0; chunkX < worldSize; chunkX++) {
        for (int chunkZ = 0; chunkZ < worldSize; chunkZ++) {;
            worldChunks[chunkX][chunkZ]->CheckCulling(_camera);
            if (worldChunks[chunkX][chunkZ]->ChunkVisible())
                displayingChunks += 1;
        }
    }

//    printf("DISPLAYING %d / %d CHUNKS\n", displayingChunks, nChunks);
}



/*
 * SKYBOX
 */

void World::SetSkyboxProperties(const Player& player) {
    // Determine max distance for skybox
    std::pair<float, float> minMax = player.GetUsingCamera()->GetMinMaxDistance();
    double maxSqrd = std::pow(minMax.second, 2.0);

    // Set skybox scale
    skyboxTransformation.SetScale({float(sqrt(maxSqrd)), float(sqrt(maxSqrd)), float(sqrt(maxSqrd))});

    // Set skybox position centred on the player
    SetSkyboxPosition(player.GetPosition());
}

void World::SetSkyboxPosition(glm::vec3 _position) {
    glm::vec3 originFromCentre;

    // Skybox
    originFromCentre = glm::vec3{_position - (skyboxTransformation.GetLocalScale() / 2.0f)};
    originFromCentre.y += skyboxTransformation.GetLocalScale().y;
    skyboxTransformation.SetPosition(originFromCentre);
    skyboxTransformation.UpdateModelMatrix();

    // Sun
    originFromCentre = _position - sunTransformation.GetLocalScale() / 2.0f;
    originFromCentre.y += sunTransformation.GetLocalScale().y;

    glm::mat4 sunAngle = glm::rotate(glm::mat4(1.0f), glm::radians(-((float)worldTime-(7*60)) * 0.125f), dirBack);
    glm::vec3 sunPos{originFromCentre.x, originFromCentre.y, originFromCentre.z};
    glm::vec3 sunOffset = sunAngle * glm::vec4(0.0f, 0.0f, 180.0f, 1.0f);

    sunTransformation.SetPosition(sunPos + sunOffset);
    sunTransformation.UpdateModelMatrix();

    // Moon
    originFromCentre = _position - moonTransformation.GetLocalScale() / 2.0f;
    originFromCentre.y += moonTransformation.GetLocalScale().y;

    glm::vec3 moonPos{originFromCentre.x, originFromCentre.y, originFromCentre.z};
    moonPos += -sunAngle * glm::vec4(0.0f, 0.0f, 180.0f, 1.0f);

    moonTransformation.SetPosition(moonPos);
    moonTransformation.UpdateModelMatrix();
}


void World::UpdateWorldTime(Uint64 _deltaTicks) {
    worldTicks += _deltaTicks;
    if (worldTicks >= 1000) {
        auto timeTicks = std::div((int)worldTicks, 1000);
        worldTime += timeTicks.quot;
        worldTicks = timeTicks.rem;
    }

    if (worldTime >= 24 * 60) {
        auto daysTime = std::div((int)worldTime, 24 * 60);
        worldDays += daysTime.quot;
        worldTime = daysTime.rem;
    }

    // Ambient lighting
    float min = 0.05f, max = 1.0f;
    int time = (int)worldTime;
    float lightLevel = max;

    if (time >= 6*60 && time < 7*60) {
        lightLevel = ((float)time - 6*60.0f) / ((7*60.0f) - (6*60.0f));
    }
    else if (time <= 19*60 && time > 18*60) {
        lightLevel = 1 - ((float)time - 18*60.0f)/ ((19*60.0f) - (18*60.0f));
    }
    else if ((time < 6*60) || (time > 19*60)) {
        lightLevel = min;
    }

    lightLevel = std::min(lightLevel, max);
    lightLevel = std::max(lightLevel, min);

    // Ambient Lighting
    GLint uLocation;
    uLocation = glGetUniformLocation(window.GetShader(), "worldAmbients.lightingStrength");
    if (uLocation < 0) printf("location not found [worldAmbients.lightingStrength]\n");
    else glUniform1f(uLocation, lightLevel);
}

/*
 * WORLD GENERATION
 */

void World::GenerateRequiredWorld() {
    GenerateTerrain();
    GenerateMeshes();
    BindChunks();
}

void World::ToggleChunkThreads(bool _threadsActive) {
    threadsActive = _threadsActive;

    // Set up the chunk builder and mesher threads for world generation
    if (threadsActive) {
        chunkBuilder = std::thread(&World::cbf, this);
        chunkMesher = std::thread(&World::cmf, this);
    }
}


float World::GenerateBlockHeight(glm::vec2 _blockPos) {
    float height;

    /*
     * PRIMARY TERRAIN LEVELS
     */

    // Seabed / ContinentBed Generation
    float continentiality = glm::simplex(glm::vec2( _blockPos.x / 2000.0, _blockPos.y / 2000.0));
    continentiality = 1;

    // Primary Noise based around waterlevel
    float baseHeight = glm::simplex(glm::vec2( _blockPos.x / 128.0, _blockPos.y / 128.0));
    baseHeight *= 5;

    // Set the base height of the block
    height = baseHeight * continentiality + MINBLOCKHEIGHT;

    // Secondary base level noise applied
    float secondHeight = glm::simplex(glm::vec2( _blockPos.x / 16.0, _blockPos.y / 16.0));
    secondHeight *= 1;
    height += secondHeight;

    /*
     *  MOUNTAIN GENERATION
     */

    // Produce noise values for mountain
    float peakHeight = glm::simplex(glm::vec2( _blockPos.x / 128.0, _blockPos.y / 128.0));
    peakHeight = (peakHeight + 1) / 2;
    peakHeight *= 128.0;

    // Determine if mountain should generate
    float areaHeight = glm::simplex(glm::vec2( _blockPos.x / 500.0, _blockPos.y / 500.0));
    areaHeight = (areaHeight + 1) / 2;

    float mountainFreq = 20; // increase to reduce number of mountains
    height += peakHeight * std::pow(areaHeight, mountainFreq);


    return std::round(height);
}

float World::GenerateBlockHeat(glm::vec3 _blockPos) {
    float heat = glm::simplex(glm::vec2(_blockPos.x / 64.0, _blockPos.z / 64.0));
    heat = (heat + 1) / 2;
    heat *= 20;
    heat += BASETEMP;

    // Relate heat to height (higher = colder)
    heat -= (_blockPos.y / MAXBLOCKHEIGHT) * 10;

    return heat;
}

float World::GenerateBlockVegetation(glm::vec3 _blockPos, float _heat) {
    float grassDensity = glm::simplex(glm::vec2( _blockPos.x / 8.0, _blockPos.z / 8.0));
    grassDensity = (grassDensity + 1) / 2;

    float treeDensity = glm::simplex(glm::vec2( _blockPos.x / 1.0, _blockPos.z / 1.0));
    treeDensity = (treeDensity + 1) / 2;
    treeDensity = std::pow(treeDensity, 10.0f);

    return grassDensity + treeDensity;
}

// Generate the height and temp maps for the given chunk starting pos
ChunkData World::GenerateChunkData(glm::vec2 _chunkPosition) {
    int chunkX = (int)_chunkPosition.x * chunkSize;
    int chunkZ = (int)_chunkPosition.y * chunkSize;
    ChunkData chunkData {};

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int bx = chunkX + x, bz = chunkZ + z;

            // Get the toplevel (highest y) of the given x z position
            float height = GenerateBlockHeight({bx, bz});
            chunkData.heightMap[x + z * chunkSize] = height;

            // Get the heat value for the block
            float heat = GenerateBlockHeat({bx,height,bz});
            chunkData.heatMap[x + z * chunkSize] = heat;

            // Create block vegetation value (relate to heat, height)
            float vegetation = GenerateBlockVegetation({bx, height, bz}, heat);
            chunkData.plantMap[x + z * chunkSize] = vegetation;
        }
    }

    return chunkData;
}



/*
 * Ensure that a biome of type BIOMEID has been generated for the world
 */

Biome* World::GenerateBiome(BIOMEID _biomeID) {
    // If the biome has been generated before then exit
    for (auto& uniqueBiome : uniqueBiomes) {
        if (uniqueBiome->GetBiomeID() == _biomeID) return uniqueBiome.get();
    }

    // Else required to create a new unique biome
    uniqueBiomes.emplace_back(CreateBiome(_biomeID));
    return uniqueBiomes.back().get();
}


void World::SetLoadingOrigin(const glm::vec3 &_origin) {
    loadingChunk = {1000 + _origin.x, 1000 + _origin.z};  // for centre chunk
}


/*
 * Generates the terrain around a given chunk. This chunk should be the chunk the player is within.
 */

void World::GenerateTerrain() {
    // prevent against loadingChunk changing during the function's runtime
    glm::ivec2 threadLoadingChunk = loadingChunk;

    // Ensure that a chunk object is created for the whole area (if not already existing)
    for (int x = -loadRadius; x < loadRadius + 1; x++) {
        for (int z = -loadRadius; z < loadRadius + 1; z++) {
            if (GetChunkAtIndex({x + threadLoadingChunk.x, 0, z + threadLoadingChunk.y}) != nullptr)
                continue;

            // Get Chunk ChunkData
            glm::vec3 chunkPos{threadLoadingChunk.x - 1000 + x, 0, threadLoadingChunk.y - 1000 + z};
            ChunkData chunkData = GenerateChunkData({chunkPos.x, chunkPos.z});
            chunkData.biome = GenerateBiome(GetBiomeIDFromData(chunkData));

            worldChunks[x + threadLoadingChunk.x][z + threadLoadingChunk.y] = std::make_unique<Chunk>(chunkPos, chunkData);
        }
    }

    chunkSumTicksTaken = 0;
    nChunksCreated = 0;

    // Assign neighbouring chunks to created chunks and invoke terrain and mesh generation
    for (int x = -loadRadius; x < loadRadius + 1; x++) {
        for (int z = -loadRadius; z < loadRadius + 1; z++) {
            auto st = SDL_GetTicks64();
            glm::vec3 chunkPos = {x + threadLoadingChunk.x, 0, z + threadLoadingChunk.y};

            // FRONT, FRONTLEFT, LEFT, BACKLEFT, BACK, BACKRIGHT, RIGHT, FRONTRIGHT
            std::array<Chunk *, 8> adjacentChunks{nullptr};
            for (int dir = 2; dir < numDirections; dir++)
                adjacentChunks[dir - 2] = GetChunkAtIndex(chunkPos + allDirections[dir]);

            Chunk* chunk = GetChunkAtIndex(chunkPos);
            if (chunk == nullptr) continue;

            // Generate new chunks
            chunk->SetAdjacentChunks(adjacentChunks);
            if (!chunk->Generated()) {
                chunk->GenerateChunk();

                auto et = SDL_GetTicks64();

                chunkSumTicksTaken += et - st;
                nChunksCreated++;
                chunkAvgTicksTaken = chunkSumTicksTaken / nChunksCreated;
            }

            // Mark chunk
        }
    }

    if (nChunksCreated != 0) {
        printf("%d CHUNKS LOADED IN %llu TICKS | AVG TICKS PER CHUNK %llu\n",
               nChunksCreated, chunkSumTicksTaken, chunkAvgTicksTaken);
    }
}


void World::GenerateMeshes() {
    // prevent against loadingChunk changing during the function's runtime
    glm::ivec2 threadLoadingChunk = loadingChunk;

    meshSumTicksTaken = 0;
    nMeshesCreated = 0;

    for (int x = -loadRadius + 1; x < loadRadius; x++) {
        for (int z = -loadRadius + 1; z < loadRadius; z++) {
            auto st = SDL_GetTicks64();
            glm::vec3 chunkPos = {x + threadLoadingChunk.x, 0, z + threadLoadingChunk.y};

            Chunk* chunk = GetChunkAtIndex(chunkPos);
            if (chunk != nullptr && chunk->Generated() && chunk->NeedsMeshUpdates()) {
                chunk->CreateChunkMeshes();

                auto et = SDL_GetTicks64();

                meshSumTicksTaken += et - st;
                nMeshesCreated++;
                meshAvgTicksTaken = meshSumTicksTaken / nMeshesCreated;
            }
        }
    }

    if (nMeshesCreated != 0) {
        printf("%d CHUNKS MESHED IN %llu TICKS | AVG TICKS PER CHUNK %llu\n",
               nMeshesCreated, meshSumTicksTaken, meshAvgTicksTaken);
    }
}


void World::BindChunks() const {
    for (int x = -loadRadius + 1; x < loadRadius; x++) {
        for (int z = -loadRadius + 1; z < loadRadius; z++) {
            Chunk* chunk = GetChunkLoadRelative({x,0,z});

            if (chunk != nullptr && chunk->UnboundMeshChanges()) {
                chunk->BindChunkMeshes();
            }
        }
    }
}






Chunk* World::GetChunkAtPosition(glm::vec3 _blockPos) const {
    _blockPos /= (float)chunkSize;
    _blockPos += glm::vec3{1000,0,1000}; // centre of the worlds chunks

    if (_blockPos.x < 0 || _blockPos.x >= 2000) return nullptr;
    if (_blockPos.z < 0 || _blockPos.z >= 2000) return nullptr;

    return worldChunks[(int)_blockPos.x][(int)_blockPos.z].get();
}

Chunk *World::GetChunkAtIndex(glm::vec3 _chunkPos) const {
    if (_chunkPos.x < 0 || _chunkPos.x >= 2000) return nullptr;
    if (_chunkPos.z < 0 || _chunkPos.z >= 2000) return nullptr;

    return worldChunks[(int)_chunkPos.x][(int)_chunkPos.z].get();
}

Chunk *World::GetChunkLoadRelative(glm::vec3 _chunkPos) const {
    _chunkPos += glm::vec3{loadingChunk.x,0,loadingChunk.y}; // centre of the worlds chunks

    if (_chunkPos.x < 0 || _chunkPos.x >= 2000) return nullptr;
    if (_chunkPos.z < 0 || _chunkPos.z >= 2000) return nullptr;

    return worldChunks[(int)_chunkPos.x][(int)_chunkPos.z].get();
}

Biome* World::GetBiome(BIOMEID _biomeID) {
    // Fetch biome
    for (auto& uniqueBiome : uniqueBiomes) {
        if (uniqueBiome->GetBiomeID() == _biomeID) return uniqueBiome.get();
    }

    // Biome did not exist?
    printf("WARNING : BIOME NOT CREATED BEFORE USAGE REQUEST\n");
    uniqueBiomes.emplace_back(CreateBiome(_biomeID));
    return uniqueBiomes.back().get();
}