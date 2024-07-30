//
// Created by cew05 on 29/07/2024.
//

#ifndef UNTITLED7_BIOME_H
#define UNTITLED7_BIOME_H

#include "../BlockModels/Block.h"

// UNIVERSAL CONSTANTS FOR GENERATION
inline const int WATERLEVEL = 35;


static const int chunkSize = 16; // must be power of 2 for subchunk division ie 2, 4, 8, 16 | 32, 64, 128, ... ( too big)
static const int chunkArea = chunkSize * chunkSize;
static const int chunkVolume = chunkArea * chunkSize;


enum BIOMEID : int {
        HILLS, MARSHLANDS, MOUNTAINS, SWAMP, PLAINS, // ...
        numBiomes
};

class Biome {
    protected:
        BIOMEID biomeID = HILLS;

        // Biome Generation values
        float noiseMultiplier = 16.0f;
        int minHeight = 35;
        double scaleX = 64.0f;
        double scaleY = 64.0f;

        // Secondary Generation values


        // BlockType height ranges

    public:
        Biome();
        ~Biome();

        [[nodiscard]] std::array<int, chunkArea> ChunkHeightMap(glm::vec3 _chunkOrigin) const;
        BlockType GetBlockType(float _hmTopLevel, float _blockY);

        [[nodiscard]] BIOMEID GetBiomeID() const { return biomeID; }
};

class Marshlands : public Biome {
    private:

    public:
        Marshlands();
};

class Mountains : public Biome {
    private:

    public:
        Mountains();
};


#endif //UNTITLED7_BIOME_H
