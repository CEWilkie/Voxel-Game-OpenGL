//
// Created by cew05 on 29/07/2024.
//

#ifndef UNTITLED7_BIOME_H
#define UNTITLED7_BIOME_H

#include "../BlockModels/Block.h"

#include "WorldGenConsts.h"

enum BIOMEID : int {
        HILLS, SWAMP, MOUNTAINS, MARSH, PLAINS, OCEAN, BEACH, // ...
        UNSPEC, numBiomes
};

enum class BIOMEATTRIB {
        HEIGHT, TEMP, // ...
};

class Biome {
    protected:
        BIOMEID biomeID = UNSPEC;

        // Generation Requirements
        float minHeight = 0;
        float minTemp = 0;

        // BlockType domains
        // ...


    public:
        Biome();
        ~Biome();

        // Getters
        [[nodiscard]] virtual BlockType GetBlockType(float _hmTopLevel, float _blockDensity, float _blockY);
        [[nodiscard]] BIOMEID GetBiomeID() const { return biomeID; }
        [[nodiscard]] float GetAttribute(BIOMEATTRIB _attribute) const;
};

class Hills : public Biome {
    private:

    public:
        Hills();
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

class Plains : public Biome {
    private:

    public:
        Plains();
};

class Beach : public Biome {
    private:

    public:
        Beach();
};

class OceanShores : public Biome {
    private:

    public:
        OceanShores();
};



#endif //UNTITLED7_BIOME_H
