//
// Created by cew05 on 23/07/2024.
//

#include "TextureManager.h"

TextureManager::TextureManager() {
    // Load all texture sheets into the textureSheet map



}

TextureManager::~TextureManager() {
    // clear all textures within the texture map from memory
    for (auto& texture : textureSheetMap) {
        texture.second.reset();
    }
}

