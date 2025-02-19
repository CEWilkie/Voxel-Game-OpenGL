//
// Created by cew05 on 31/07/2024.
//

#ifndef UNTITLED7_CREATEBIOME_H
#define UNTITLED7_CREATEBIOME_H

#include <numeric>

#include "Biome.h"
#include "../WorldGenConsts.h"

/*
 * GET BIOME ID FOR THE PROVIDED CHUNKDATA
 */

inline Biome::ID GetBiomeIDFromData(const ChunkData& _chunkData) {
    // retrieve average values for the chunk
    auto averageHeight = std::accumulate(_chunkData.heightMap.begin(), _chunkData.heightMap.end(), 0.0f) / chunkArea;
//    auto averageHeat = std::accumulate(_chunkData.heatMap.begin(), _chunkData.heatMap.end(), 0.0f) / chunkArea;

    // In order of importance : Height ...
    Biome::ID biomeID;
    using enum Biome::ID;

    if (averageHeight >= WATERLEVEL + 30) biomeID = MOUNTAINS;
    else if (averageHeight >= WATERLEVEL + 10) biomeID = HILLS;
    else if (averageHeight >= WATERLEVEL + 3) biomeID = PLAINS;
//    else if (height >= WATERLEVEL + 1) biomeID = SWAMP;
    else if (averageHeight >= WATERLEVEL) biomeID = BEACH;
    else biomeID = OCEAN;


    return biomeID;
}




/*
 * CONSTRUCT ACTUAL BIOME FROM BIOMEID
 */

inline std::unique_ptr<Biome> CreateBiome(Biome::ID _biomeID) {
    Biome* biome;
    using enum Biome::ID;

    switch (_biomeID) {
        case MOUNTAINS:
            biome = new Mountains();
            break;

        case HILLS:
            biome = new Hills();
            break;

        case PLAINS:
            biome = new Plains();
            break;

        case BEACH:
            biome = new Beach();
            break;

        case OCEAN:
            biome = new OceanShores();
            break;

        default:
            biome = new Biome();
    }

    return std::make_unique<Biome>(*biome);
}

/*
 * CONSTRUCT ACTUAL BIOME FROM THE PROVIDED CHUNKDATA
 */

inline std::unique_ptr<Biome> CreateBiome(const ChunkData& _chunkData) {
    // Fetch biomeID and create biome from that
    return CreateBiome(GetBiomeIDFromData(_chunkData));
}

#endif //UNTITLED7_CREATEBIOME_H
