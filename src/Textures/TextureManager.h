//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_TEXTUREMANAGER_H
#define UNTITLED7_TEXTUREMANAGER_H

/*
 * Set enums for the texture sheets
 */

enum class TEXTURESHEET {
    NATURAL, WORLD, TEST16, TEST64,
};

/*
 * Set enums for individual textures
 */

enum class TEXTURE {
        STONE, DIRT, GRASS
};

#include <unordered_map>
#include <memory>

#include "TextureData.h"

struct Texture {
    TEXTURESHEET sheetID;
    std::pair<int, int> textureGridOrigin;
};


class TextureManager {
    private:
        std::unordered_map<TEXTURESHEET, std::unique_ptr<TextureData>> textureSheetMap;
        std::unordered_map<TEXTURE, Texture> textureMap;

    public:
        // Con/Destructor
        TextureManager();
        ~TextureManager();

        //

};


// Global texture manager which can be used by other objects int the program to fetch textures from
inline std::unique_ptr<TextureManager> textureManager = std::make_unique<TextureManager>();

#endif //UNTITLED7_TEXTUREMANAGER_H
