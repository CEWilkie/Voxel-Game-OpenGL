//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_TEXTUREMANAGER_H
#define UNTITLED7_TEXTUREMANAGER_H

/*
 * Set enums for the texture sheets
 */

enum class TEXTURESHEET {
    TERRAIN, NATURAL, WORLD, TEST16, TEST64,
};

#include <unordered_map>
#include <memory>

#include "TextureData.h"

class TextureManager {
    private:
        std::unordered_map<TEXTURESHEET, std::unique_ptr<TextureData>> textureSheetMap;

    public:
        // Con/Destructor
        TextureManager();
        ~TextureManager();

        // Enable and Disable the specific texture sheets
        void EnableTextureSheet(TEXTURESHEET _sheetID);
        void DisableTextureSheet(TEXTURESHEET _sheetID);

        TextureData* GetTextureData(TEXTURESHEET _sheetID);
};


// Global texture manager which can be used by other objects int the program to fetch textures from
inline std::unique_ptr<TextureManager> textureManager;

#endif //UNTITLED7_TEXTUREMANAGER_H
