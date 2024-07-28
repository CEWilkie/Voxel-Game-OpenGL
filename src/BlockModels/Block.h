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
        TEST, GRASS, DIRT, STONE, WATER, AIR,
};

enum BLOCKFACE : int{
        FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, ALL
};

struct BlockData {
    // Used to identify what the actual block object is of
    BLOCKID blockID {BLOCKID::AIR};
    int variantID {0};
//    glm::vec3 position;
//    std::array<BLOCKFACE, 6> visibleFaces;

    static bool Compare(BlockData A, BlockData B) {
        return A.blockID == B.blockID && A.variantID == B.variantID;
    }
};

enum BLOCKMODEL {
    FULL, nModels
};

struct BlockVAOs {
    // Buffer objects
    std::array<GLuint, nModels> vertexArrayObject {};
    std::array<GLuint, nModels> vertexBufferObject {};

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



class Block {
    protected:
        // Block Buffers
        unsigned int indexBufferObject {};

        // Block Culling
        bool inCamera = true, culled = false;
        std::vector<BLOCKFACE> visibleFaces {TOP, BOTTOM, FRONT, BACK, LEFT, RIGHT};

        // Block Display
        Transformation* blockTransformation {};
        TEXTURESHEET sheet {TEXTURESHEET::WORLD};
        glm::vec2 origin {1,1};

        // Block Data
        BlockData blockData {AIR, 0};
        BLOCKMODEL blockModel {FULL};

    public:
        Block();
        ~Block();

        void UpdateIndexBuffer();

        // Model Display and Transformation
        void Display();
        void SetTransformation(Transformation* _t);

        // Culling
        void CheckCulling(const Camera& _camera);
        void HideFace(BLOCKFACE _face);
        void HideFaces(const std::vector<BLOCKFACE>& _faces);

        // Getters
        [[nodiscard]] BlockData GetBlockData() const { return blockData; }
        [[nodiscard]] bool IsCulled() const {return culled; }
        [[nodiscard]] std::vector<Vertex> GetFaceVerticies(const std::vector<BLOCKFACE>& _faces) const;
        [[nodiscard]] std::vector<BLOCKFACE> GetVisibleFaces() const { return visibleFaces; }
        [[nodiscard]] glm::vec2 GetTextureOrigin() const { return origin; }
        [[nodiscard]] TEXTURESHEET GetTextureSheet() const { return sheet; }
};








class TestBlock : public Block {
    private:

    public:
        explicit TestBlock(int _variant);
};



#endif //UNTITLED7_BLOCK_H
