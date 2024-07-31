//
// Created by cew05 on 26/07/2024.
//

#ifndef UNTITLED7_CREATEBLOCK_H
#define UNTITLED7_CREATEBLOCK_H

#include "NaturalBlocks.h"

// Create a new block instance from a particular ID and Variant upon request
inline std::unique_ptr<Block> CreateBlock(BlockType _blockData) {
    std::unique_ptr<Block> newBlock {};

    BLOCKID _id = _blockData.blockID;
    int _variant = _blockData.variantID;

    // In order of likelihood to appear to save search time
    switch (_id) {
        case BLOCKID::AIR:
            newBlock = std::make_unique<Air>(_variant);
            break;

        case BLOCKID::STONE:
            newBlock = std::make_unique<Stone>(_variant);
            break;

        case BLOCKID::DIRT:
            newBlock = std::make_unique<Dirt>(_variant);
            break;

        case BLOCKID::WATER:
            newBlock = std::make_unique<Water>(_variant);
            break;

        case BLOCKID::GRASS:
            newBlock = std::make_unique<Grass>(_variant);
            break;

        case BLOCKID::SAND:
            newBlock = std::make_unique<Sand>(_variant);
            break;

        default:
            // Block not defined
            newBlock = std::make_unique<TestBlock>(_variant);
    }

    return newBlock;
}

#endif //UNTITLED7_CREATEBLOCK_H
