//
// Created by cew05 on 10/07/2024.
//

#ifndef UNTITLED7_TEXTURE_H
#define UNTITLED7_TEXTURE_H

#include <string>
#include <glew.h>
#include <glm/vec2.hpp>

class Texture {
    private:
        // Texture buffer
        GLuint textureBuffer {};
        GLuint pixelbuffer {};

        std::string texturePath {};
        std::pair<int, int> textureSheetSize {};
        std::pair<int, int> textureTileSize {};
        std::pair<float, float> textureGrid {};

    public:
        explicit Texture(const std::string& _texturePath);
        ~Texture();

        void Path() const { printf("path: %s\n", texturePath.c_str()); };

        // Using tilesheets / texturesheets
        void SetTextureSheetGrid(std::pair<float, float> _textureGrid);
        [[nodiscard]] glm::vec2 GetTextureSheetTile(glm::vec2 _gridPos) const {
            return {_gridPos.x / textureGrid.first, _gridPos.y / textureGrid.second};
        };

        // Activating the texture
        void EnableTexture() const;
        void DisableTexture() const;
};


#endif //UNTITLED7_TEXTURE_H
