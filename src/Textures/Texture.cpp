//
// Created by cew05 on 10/07/2024.
//

#include "TextureData.h"

#include <SDL.h>
#include <SDL_image.h>

#include "../ErrorLogging.h"

TextureData::TextureData(const std::string& _texturePath) {
    // store path
    texturePath = _texturePath;

    // Generate new texture ID
    glGenTextures(1, &textureObject);
    glGenBuffers(1, &textureBuffer);

    // Load image to surface
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (surface == nullptr) {
        LogError("Failed to load texture", SDL_GetError());
        return;
    }

    // Set texture dimensions from surface, assume not a texture sheet
    textureSheetSize = {surface->w, surface->h};
    textureTileSize = {surface->w, surface->h};
    textureGrid = {1, 1};

    // Get image format
    Uint8 nColours = surface->format->BytesPerPixel;
    GLenum textureFormat;
    GLint internalFormat;
    if (nColours == 4) { // Contains alpha
        if (surface->format->Rmask == 0x000000ff) textureFormat = GL_RGBA;
        else textureFormat = GL_BGRA;
        internalFormat = GL_RGBA8;
    }
    else if (nColours == 3) { // No alpha
        if (surface->format->Rmask == 0x000000ff) textureFormat = GL_RGB;
        else textureFormat = GL_BGR;
        internalFormat = GL_RGB8;
    }
    else {
        printf("Image potentially unsuitable, only %d colour channels.\n Ending texture assignment\n", nColours);
        return;
    }

    // bind to the texture object
    glBindTexture(GL_TEXTURE_2D, textureObject);

    // Pixel Alignment Info
    int alignment = 8;
    while (surface->pitch%alignment) alignment>>=1; // x%1==0 for any x
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);

    // Set texture stretch properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // TextureData wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // TextureData environment interactions
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Store image data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0,
                 textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind
    SDL_FreeSurface(surface);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureData::~TextureData() {
    // Clear buffer and texture data
    glDeleteTextures(1, &textureObject);
    glDeleteBuffers(1, &textureBuffer);
}


void TextureData::SetTextureSheetGrid(std::pair<float, float> _textureGrid) {
    if (_textureGrid.first == 0 || _textureGrid.second == 0) {
        printf("INVALID GRID SIZE %f %f\n", _textureGrid.first, _textureGrid.second);
        return;
    }

    textureGrid = _textureGrid;
    textureTileSize = {textureSheetSize.first / int(textureGrid.first),
                       textureSheetSize.second / int(textureGrid.second)};

    if ((textureTileSize.first & (textureTileSize.first - 1)) != 0 ||
    (textureTileSize.second & (textureTileSize.second - 1)) != 0 ) {
        // NOT A POWER OF TWO SIZE, OR 0 SIZE!
        printf("WARNING: TEXTURE HAS NON-POWER-2 DIMENSIONS OR HAS SIZE 0\nSIZE: %d, %d",
               textureTileSize.first, textureTileSize.second);
    }
}

void TextureData::EnableTexture() const {
    glBindTexture(GL_TEXTURE_2D, textureObject);
}

void TextureData::DisableTexture() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}