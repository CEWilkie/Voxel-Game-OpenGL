//
// Created by cew05 on 23/07/2024.
//

#include "TextureManager.h"

#include <filesystem>

TextureManager::TextureManager() {
    // Load all texture sheets into the textureSheet map
    TextureData* textureData;

    // test textures for objects

    textureData = new TextureData("../resources/testcubes16x.png");
    if (textureData->IsTextureValid()) {
        textureData->SetTextureSheetGrid({16, 16});
        textureSheetMap[TEXTURESHEET::TEST16] = std::make_unique<TextureData>(*textureData);
    }

    // skybox textures

    textureData = new TextureData("../resources/skybox16x.png");
    if (textureData->IsTextureValid()) {
        textureData->SetTextureSheetGrid({16, 16});
        textureSheetMap[TEXTURESHEET::WORLD] = std::make_unique<TextureData>(*textureData);
    }

    // Terrain block textures

    textureData = new TextureData("../resources/terrainSheet16x.png");
    if (textureData->IsTextureValid()) {
        textureData->SetTextureSheetGrid({16, 16});
        textureSheetMap[TEXTURESHEET::TERRAIN] = std::make_unique<TextureData>(*textureData);
    }

    // Natural block textures

    textureData = new TextureData("../resources/naturalSheet16x.png");
    if (textureData->IsTextureValid()) {
        textureData->SetTextureSheetGrid({16, 16});
        textureSheetMap[TEXTURESHEET::NATURAL] = std::make_unique<TextureData>(*textureData);
    }

    // ...

    // Simple log to check how many textures have been created
    printf("OPENED %zu TEXTURE SHEETS\n", textureSheetMap.size());
}

TextureManager::~TextureManager() {
    // clear all textures within the texture map from memory
    for (auto& texture : textureSheetMap) {
        texture.second.reset();
    }
}


void TextureManager::EnableTextureSheet(TEXTURESHEET _sheetID) {
    // ensure sheet has been constructed in the map
    if (textureSheetMap.count(_sheetID) < 1) return;

    textureSheetMap[_sheetID]->EnableTexture();
}

void TextureManager::DisableTextureSheet(TEXTURESHEET _sheetID) {
    // ensure sheet has been constructed in the map
    if (textureSheetMap.count(_sheetID) < 1) return;
    textureSheetMap[_sheetID]->DisableTexture();
}

TextureData* TextureManager::GetTextureData(TEXTURESHEET _sheetID) {
    // ensure sheet has been constructed in the map
    if (textureSheetMap.count(_sheetID) < 1) return nullptr;

    return textureSheetMap[_sheetID].get();
}