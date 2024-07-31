//
// Created by cew05 on 07/07/2024.
//

#ifndef UNTITLED7_BLOCK_H
#define UNTITLED7_BLOCK_H

#include <memory>
#include <vector>
#include <string>

#include <glew.h>
#include <glm/matrix.hpp>

#include "../Textures/TextureData.h"
#include "ModelStructs.h"
#include "ModelTransformations.h"
#include "../Player/Camera.h"
#include "../Textures/TextureManager.h"

enum BLOCKID : unsigned int {
        TEST, GRASS, DIRT, STONE, WATER, AIR, SAND,
};

enum BLOCKFACE : int{
        FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, ALL
};

struct BlockType {
    // Used to identify what the actual block object is of
    BLOCKID blockID {BLOCKID::AIR};
    int variantID {0};

    static bool Compare(BlockType A, BlockType B) {
        return A.blockID == B.blockID && A.variantID == B.variantID;
    }
};

enum BLOCKMODEL {
    FULL, nModels
};

/*
 * A struct that binds the block models into memory, and permits the fetching / retrieving of a particular model's
 * VAO for display, or the base verticies / indexes of a model
 */

struct BlockVAOs {
    // Buffer objects
    std::array<GLuint, nModels> vertexArrayObject {};
    std::array<GLuint, nModels> vertexBufferObject {};
    std::array<GLuint, nModels> indexBufferObject {};

    BlockVAOs();
    ~BlockVAOs();

    // VAO Creation for FullBlock
    static std::vector<Vertex> FullblockVA();
    static std::vector<GLuint> FullblockIA();

    // ...

    // Bind the block vertex data
    void BindBlockModels() const;

    // Getters for unchanged IndexArrays / VertexArrays
    [[nodiscard]] static std::vector<GLuint> GetBaseIndexArray(BLOCKMODEL _model) ;
    [[nodiscard]] static std::vector<Vertex> GetBaseVertexArray(BLOCKMODEL _model) ;
};

inline std::unique_ptr<BlockVAOs> blockVAOmanager {};






enum class BLOCKATTRIBUTE {
        TRANSPARENT, // ... other block attributes
};

class Block {
    protected:
        // Block Culling
        bool inCamera = true, culled = false;
        std::vector<BLOCKFACE> visibleFaces {TOP, BOTTOM, FRONT, BACK, LEFT, RIGHT};

        // Block Display
        TEXTURESHEET sheet {TEXTURESHEET::WORLD};
        glm::vec2 origin {1,1};

        // Block Data
        int transparent = 0;

        // BlockType info
        BlockType blockData {AIR, 0};
        BLOCKMODEL blockModel {FULL};

    public:
        Block();
        ~Block();

        // Model Display and Transformation
        void Display(Transformation* _t);
        void SetTransformation(Transformation* _t);

        // Getters
        [[nodiscard]] BlockType GetBlockType() const { return blockData; }
        [[nodiscard]] bool IsCulled() const {return culled; }
        [[nodiscard]] glm::vec2 GetTextureOrigin() const { return origin; }
        [[nodiscard]] TEXTURESHEET GetTextureSheet() const { return sheet; }
        [[nodiscard]] std::vector<Vertex> GetFaceVerticies(const std::vector<BLOCKFACE>& _faces) const;
        [[nodiscard]] int GetAttributeValue(BLOCKATTRIBUTE _attribute) const;
};








class TestBlock : public Block {
    private:

    public:
        explicit TestBlock(int _variant);
};



#endif //UNTITLED7_BLOCK_H
