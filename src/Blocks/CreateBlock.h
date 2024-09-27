//
// Created by cew05 on 26/07/2024.
//

#ifndef UNTITLED7_CREATEBLOCK_H
#define UNTITLED7_CREATEBLOCK_H

#include "TerrainBlocks.h"
#include "NaturalBlocks.h"

// Create a new block instance from a particular ID and Variant upon request
inline std::unique_ptr<Block> CreateBlock(BlockType _blockData) {
    std::unique_ptr<Block> newBlock {};

    BLOCKID id = _blockData.blockID;
    GLbyte variant = _blockData.variantID;

    // In order of likelihood to appear to save search time
    switch (id) {
        case BLOCKID::AIR:
            newBlock = std::make_unique<Air>(variant);
            break;

        case BLOCKID::STONE:
            newBlock = std::make_unique<Stone>(variant);
            break;

        case BLOCKID::DIRT:
            newBlock = std::make_unique<Dirt>(variant);
            break;

        case BLOCKID::WATER:
            newBlock = std::make_unique<Water>(variant);
            break;

        case BLOCKID::GRASS:
            newBlock = std::make_unique<Grass>(variant);
            break;

        case BLOCKID::SAND:
            newBlock = std::make_unique<Sand>(variant);
            break;

        case BLOCKID::LEAVES:
            newBlock = std::make_unique<Leaves>(variant);
            break;

        case BLOCKID::GRASSPLANT:
            newBlock = std::make_unique<GrassPlant>(variant);
            break;

        case BLOCKID::WOOD:
            newBlock = std::make_unique<Wood>(variant);
            break;

        case BLOCKID::UNBREAKABLEBLOCK:
            newBlock = std::make_unique<UnbreakableBlock>(variant);
            break;

        default:
            // Block not defined
            newBlock = std::make_unique<TestBlock>(variant);
    }

    return newBlock;
}

#endif //UNTITLED7_CREATEBLOCK_H
