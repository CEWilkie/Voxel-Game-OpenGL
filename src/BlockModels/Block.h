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
#include "../Textures/TextureManager.h"
#include "ModelStructs.h"
#include "ModelTransformations.h"

/*
 * For obtaining a particular block model
 */

enum BLOCKMODEL {
    FULL, nModels
};

/*
 * For referencing a particular face of a block
 */

enum BLOCKFACE : int {
    FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, ALL
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



/*
 * Base-level identification of block types. Shared between variants
 */

enum BLOCKID : unsigned int {
    TEST, GRASS, DIRT, STONE, WATER, AIR, SAND, LEAVES,
};



/*
 * Used to identify the type of block through the combination of BLOCKID and a variant value. Blocks should only be
 * considered as the same type if they share both blockID and variantID
 */

struct BlockType {
    BLOCKID blockID {BLOCKID::AIR};
    int variantID {0};

    static bool Compare(const BlockType& A, const BlockType& B) {
        return A.blockID == B.blockID && A.variantID == B.variantID;
    }

    friend bool operator==(const BlockType& A, const BlockType& B) {
        return Compare(A, B);
    }
};



/*
 * Enums for each attribute that a block uses. Attributes when applied to a block should refer to an int value which
 * can be obtained and utilised. For example, transparency = 0 indicates a solid, non-transparent block. Wheras a
 * transparency = 1 indicates a block with transparent elements. Values are not necessarily limited to 0 or 1 but must
 * be int
 */

enum class BLOCKATTRIBUTE {
    TRANSPARENT, LIQUID, BREAKABLE, CANACCESSTHROUGHBLOCK, FACINGDIRECTION, ROTATION,
    // ... other block attributes
};



/*
 * Data struct to contain block attributes which are not equivalent within all instances of a block type. Whilst all
 * grass blocks are solid (hence the attribute is stored in the block object), they may face in a different direction,
 * and thus this is stored in BlockAttributes.
 */

struct BlockAttributes {
    int rotation = 0;
    int topFaceDirection = DIRECTION::UP;

    [[nodiscard]] int GetAttributeValue(BLOCKATTRIBUTE _attribute) const;
};



/*
 * A block!
 */

class Block {
    protected:
        // Block Display
        TEXTURESHEET sheet {TEXTURESHEET::WORLD};
        glm::vec2 origin {1,1};

        // Block Data Attributes
        int transparent = 0;
        int liquid = 0;
        int breakable = 1;
        int canInteractThroughBlock = 0;

        // Visual Rotations
        bool topFaceLocked = true; // can only face up
        bool rotationLocked = false; // cannot be rotated

        // BlockType info
        BlockType blockData {AIR, 0};
        BLOCKMODEL blockModel {FULL};

    public:
        Block();
        ~Block();

        // Model Display
        void Display(Transformation* _t);
        void DisplayWireframe(Transformation* _transformation);
        [[nodiscard]] glm::vec2 GetTextureOrigin() const { return origin; }
        [[nodiscard]] TEXTURESHEET GetTextureSheet() const { return sheet; }

        // BlockAttributes
        [[nodiscard]] BlockType GetBlockType() const { return blockData; }
        [[nodiscard]] int GetAttributeValue(BLOCKATTRIBUTE _attribute) const;

        // Block Face Culling
        [[nodiscard]] DIRECTION GetRandomTopFaceDirection() const;
        [[nodiscard]] int GetRandomRotation() const;
        [[nodiscard]] std::vector<Vertex> GetFaceVerticies(const std::vector<BLOCKFACE>& _faces, const BlockAttributes& _blockAttributes) const;
};






/*
 * Testing block, used for testing. Should not naturally generate. Could also be used if a request for a block with
 * faulty data is made. TestBlock can be the default return.
 */

class TestBlock : public Block {
    private:

    public:
        explicit TestBlock(int _variant) {
            blockData = {BLOCKID::TEST, _variant};
            sheet = TEXTURESHEET::TEST16;

            // Set texture origin
            switch (_variant) {
                case 1: origin = {4,2}; break;
                case 2: origin = {5,4}; break;
                case 3: origin = {7,1}; break;
                case 4: origin = {10,2}; break;
                case 5: origin = {13,1}; break;
                default: origin = {1,1};
            }
        };
};



#endif //UNTITLED7_BLOCK_H
