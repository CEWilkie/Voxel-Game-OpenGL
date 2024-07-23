//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_TEXTUREDATA_H
#define UNTITLED7_TEXTUREDATA_H

#include <string>
#include <glew.h>
#include <glm/vec2.hpp>

class TextureData {
    private:
        // TextureData buffer
        GLuint textureObject {};
        GLuint textureBuffer {};

        std::string texturePath {};
        std::pair<int, int> textureSheetSize {};
        std::pair<int, int> textureTileSize {};
        std::pair<float, float> textureGrid {};

        // Checker vars
        bool isGood = false; // Checker var to ensure texture data is fully constructed
        bool isBound = false; // Checker var for status of the TextureData being bound

    public:
        explicit TextureData(const std::string& _texturePath);
        ~TextureData();

        // Using tilesheets / texturesheets
        void SetTextureSheetGrid(std::pair<float, float> _textureGrid);
        [[nodiscard]] glm::vec2 GetTextureSheetTile(glm::vec2 _gridPos) const {
            return {_gridPos.x / textureGrid.first, _gridPos.y / textureGrid.second};
        };

        // Activating the texture
        void EnableTexture();
        void DisableTexture();

        // Checking texture states
        [[nodiscard]] bool IsTextureValid() const { return isGood; };
        [[nodiscard]] bool IsTextureBound() const { return isBound; };
};

#endif //UNTITLED7_TEXTUREDATA_H
