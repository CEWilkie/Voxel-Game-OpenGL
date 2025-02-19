//
// Created by cew05 on 18/08/2024.
//

#ifndef UNTITLED7_LOADSTRUCTURE_H
#define UNTITLED7_LOADSTRUCTURE_H

#include "../BlockModels/Block.h"

#include <glm/vec3.hpp>
#include <fstream>
#include <filesystem>

/*
 * Struct holds the block data which has been read from the structure file. BlockPos is relative to the origin of the
 * structure.
 */

struct StructBlockData {
    glm::vec3 blockPos {0,0,0};
    BlockType blockType {AIR, 0};
};


/*
 * Struct Contains data for each structure
 */

struct StructureData {
    public:
        typedef std::vector<StructBlockData> StructBlocks;

        StructureData() = default;
        StructureData(const std::string& _name, const StructBlocks& _structBlocks) {
            structureName = _name;
            structBlocks = _structBlocks;
        }

        [[nodiscard]] StructBlockData Get(int _index) const {
            if (_index < 0 || _index >= size()) return {};
            return structBlocks[_index];
        }

        [[nodiscard]] size_t size() const { return structBlocks.size(); }
        [[nodiscard]] std::string Name() const { return structureName; }

    private:
        std::string structureName;
        StructBlocks structBlocks {};
};


/*
 * Class loads the blocks of a structure from a csv file of block position and type data. After all blocks are given to
 * the structure-loading chunk, loadedStructure is set to NONE - which indicates that all blocks have been retrieved.
 */

class StructureLoader {
    public:
        typedef std::vector<StructBlockData> StructBlocks;

        StructureLoader();
        ~StructureLoader();

        // Retrieve Structure Blocks
        bool validateStructureName(const std::string& _structureName) const;
        [[nodiscard]] StructureData GetStructureData(const std::string& _structureName) const;
        [[nodiscard]] size_t GetStructureSize(const std::string& _structureName) const;

        // Populate Loader
        void LoadStructures(const std::vector<std::string>& _structureFileNames);

    private:
        bool LoadStructure(const std::string& _structureFileName);

        // StructureData
        const std::string structuresDirRelPath = "./../Structures";
        std::unordered_map<std::string, StructureData> structureBlocks;
};


#endif //UNTITLED7_LOADSTRUCTURE_H
