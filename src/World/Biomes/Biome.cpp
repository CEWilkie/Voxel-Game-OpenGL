//
// Created by cew05 on 29/07/2024.
//

#include "Biome.h"

using enum Biome::ID;

Biome::Biome() {
    // Common Structures
    loader.LoadStructures({"testStruct.csv"});
}
Biome::~Biome() = default;

BlockType Biome::GetBlockType(float _hmTopLevel, float _blockY) {

    /*
     * HEIGHT-BASED TERRAIN GENERATION
     */

    // BlockType
    BlockType newBlockType = {STONE, 0};

    // SubSurface Terrain
    if (_blockY == 0) newBlockType = {UNBREAKABLEBLOCK, 0};
    else if (_blockY <= DENSITYLAYER || (_blockY < DENSITYLAYER + 5)) newBlockType = {TOUGHSTONE, 0};
    else if (_blockY < _hmTopLevel - 4) newBlockType = {STONE, 0};

    // Topsurface Terrain
    else if (_blockY <= _hmTopLevel && _blockY < WATERLEVEL + 2) newBlockType = {SAND, 0};
    else if (_blockY < _hmTopLevel && _hmTopLevel < WATERLEVEL + 120) newBlockType = {DIRT, 0};
    else if (_blockY == _hmTopLevel && _hmTopLevel < WATERLEVEL + 120) newBlockType = {GRASS, 0};
    else if (_blockY > _hmTopLevel && _blockY <= WATERLEVEL) newBlockType = {WATER, 0};

    return newBlockType;
}




Biome::FOLIAGE Biome::GetFoliage(float _plantDensity) {

    if (_plantDensity > minLargeTree) return FOLIAGE::BIG_TREE;
    if (_plantDensity > minTree) return FOLIAGE::TREE;
    if (_plantDensity > minShrub) return FOLIAGE::SHRUB;

    std::uniform_real_distribution<> flowerDistrib(0, 1);
    double flowerRand = flowerDistrib(worldGenerationRandom);

    if (_plantDensity > minLargePlant) {
        if (flowerRand > flowerRate) return FOLIAGE::LONG_GRASS;
        else return FOLIAGE::TALL_FLOWER;
    }

    if (_plantDensity > minShortPlant) {
        if (flowerRand > flowerRate) return FOLIAGE::SHORT_GRASS;
        else return FOLIAGE::FLOWER;
    }

    else return FOLIAGE::NONE;
}


BlockType Biome::BuildFoliage(Biome::FOLIAGE _foliageType, float _plantDensity, int *_height) {

    // Determine plant height
    if ((int)_foliageType < (int)FOLIAGE::PLANT_TALL) *_height = 1;
    else *_height = 1;

    // Determine plant block
    BlockType block{AIR, 0};
    switch(_foliageType) {
        case FOLIAGE::SHORT_GRASS:
        case FOLIAGE::FLOWER:
        case FOLIAGE::LONG_GRASS:
        case FOLIAGE::TALL_FLOWER:
            block = BlockType{GRASSPLANT, 0};
            break;

        default:
            break;
    }

    return block;
}


void Biome::LoadStructure(Biome::STRUCTURES _structure) {
    switch (_structure) {
        case STRUCTURES::TREE:
        case STRUCTURES::BIG_TREE:
            loadedStructData = loader.GetStructureData("testStruct");
            structBlocksRemaining = loadedStructData.size();
            break;

        case STRUCTURES::SHRUB:
        default:
            return;
    }

    // Invalid Structure
    if (loadedStructData.size() == 0) {
        printf("Structure name %s not Valid\n", loadedStructData.Name().c_str());
    }
}

StructBlockData Biome::BuildStructure(bool *_completed, float _solidity) {

    StructBlockData structBlock {};

    // Ensure valid structure
    if (loadedStructData.size() == 0) {
        *_completed = true;
        return structBlock;
    }

    // Structure complete?
    if (*_completed) return structBlock;


    // Fetch Struct Block
    size_t totalBlocks = loadedStructData.size();
    structBlock = loadedStructData.Get(int(totalBlocks - structBlocksRemaining));
    structBlocksRemaining -= 1;

    // non-1 solidity indicates that some blocks can be removed
    if (_solidity < 1) {
        std::uniform_real_distribution<> solidityRand(0, 1);
        double r = solidityRand(worldGenerationRandom);

        if (r > _solidity) structBlock.blockType = {AIR, 0};
    }

    if (structBlocksRemaining <= 0) {
        *_completed = true;
        loadedStructData = {};
        structBlocksRemaining = 0;
    }

    return structBlock;
}





float Biome::GetAttribute(ATTRIBUTE _attribute) const {
    switch (_attribute) {
        case ATTRIBUTE::HEIGHT:
            return minHeight;

        case ATTRIBUTE::TEMP:
            return minTemp;

        default:
            return 0.0f;
    }
}



Hills::Hills() {
    biomeID = HILLS;

    minHeight = WATERLEVEL + 10;
}

Marshlands::Marshlands() {
    biomeID = SWAMP;

    minHeight = WATERLEVEL + 1;

}


Mountains::Mountains() {
    biomeID = MOUNTAINS;

    minHeight = WATERLEVEL + 30;

}

Plains::Plains() {
    biomeID = PLAINS;

    minHeight = WATERLEVEL + 3;

    minShortPlant = 0.5f;
    minTree = 1.5f;
    minLargeTree = 2.0f;
}

Beach::Beach() {
    biomeID = BEACH;

    minHeight = WATERLEVEL;

}

OceanShores::OceanShores() {
    biomeID = OCEAN;

    minHeight = WATERLEVEL - 3;

}