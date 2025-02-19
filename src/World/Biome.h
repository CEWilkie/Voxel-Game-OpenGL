//
// Created by cew05 on 29/07/2024.
//

#ifndef UNTITLED7_BIOME_H
#define UNTITLED7_BIOME_H

#include "../BlockModels/Block.h"

#include "WorldGenConsts.h"
#include "LoadStructure.h"

class Biome {
    public:
        enum class ID {
            HILLS, SWAMP, MOUNTAINS, MARSH, PLAINS, OCEAN, BEACH, // ...
            UNSPEC, numBiomes
        };

        enum class ATTRIBUTE {
            HEIGHT, TEMP, // ...
        };

        enum class FOLIAGE : int {
            // Standard Flowers / Grass
            NONE, SHORT_GRASS, FLOWER,

            // Tall plants
            PLANT_TALL, LONG_GRASS, TALL_FLOWER,

            // Structure Type Foliage
            STRUCTURE_TYPE, SHRUB, TREE, BIG_TREE,
            END_FOLIAGE,
        };

        enum class STRUCTURES : int {
            // Foliage-type structures
            SHRUB = (int)FOLIAGE::SHRUB, TREE, BIG_TREE,

            // Minor Structures


            // Major Structures

        };

        Biome();
        ~Biome();

        // Biome Block and Decorative Foliage Generation
        [[nodiscard]] virtual BlockType GetBlockType(float _hmTopLevel, float _blockY);
        [[nodiscard]] virtual FOLIAGE GetFoliage(float _plantDensity);
        [[nodiscard]] virtual BlockType BuildFoliage(FOLIAGE _foliageType, float _plantDensity, int* _height);

        // Large Structure Gen
        void LoadStructure(STRUCTURES _structure);
        [[nodiscard]] StructBlockData BuildStructure(bool* _completed, float _solidity);

        // Getters
        [[nodiscard]] ID GetBiomeID() const { return biomeID; }
        [[nodiscard]] float GetAttribute(ATTRIBUTE _attribute) const;

    protected:
        ID biomeID = ID::UNSPEC;

        // Generation Requirements
        float minHeight = 0;
        float minTemp = 0;

        // BlockType domains
        // ...

        // Biome Foliage Structures
        StructureData loadedStructData;
        size_t structBlocksRemaining = 0;
        StructureLoader loader = StructureLoader();

        // Biome Foliage Gen Levels
        float minShrub = 0.9f, minTree = 1.0f, minLargeTree = 1.5f;
        float minShortPlant = 0.7, minLargePlant = 0.75;
        float flowerRate = 0.1f;
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
