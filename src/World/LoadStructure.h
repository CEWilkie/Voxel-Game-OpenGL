//
// Created by cew05 on 18/08/2024.
//

#ifndef UNTITLED7_LOADSTRUCTURE_H
#define UNTITLED7_LOADSTRUCTURE_H

#include "../BlockModels/Block.h"
#include <glm/vec3.hpp>
#include <fstream>

enum class STRUCTURE {
        NONE, TEST, // ...
};



/*
 * Struct holds the block data which has been read from the structure file. BlockPos is relative to the origin of the
 * structure.
 */

struct StructBlockData {
    glm::vec3 blockPos {0,0,0};
    BlockType blockType {TEST, 0};
};



/*
 * Class loads the blocks of a structure from a csv file of block position and type data. After all blocks are given to
 * the structure-loading chunk, loadedStructure is set to NONE - which indicates that all blocks have been retrieved.
 */

class StructureLoader {
    private:
        STRUCTURE loadedStructure = STRUCTURE::NONE;
        STRUCTURE lastStructure = STRUCTURE::NONE;

        std::vector<StructBlockData> structBlocks {};
        int requestedBlocks = 0;

    public:
        StructureLoader() = default;

        void StartLoadingStructure(const STRUCTURE& _structure) {
            if (loadedStructure != STRUCTURE::NONE) return;

            requestedBlocks = 0;

            // Load block data from csv if last structure was different
            loadedStructure = _structure;
            if (lastStructure != _structure) {
                structBlocks.clear();

                std::fstream structFile("../Structures/testStruct.csv");
                if (!structFile.good()) {
                    loadedStructure = STRUCTURE::NONE;
                    return;
                }

                std::string line;
                std::getline(structFile, line); // ignore first line as header
                while (std::getline(structFile, line)) {
                    StructBlockData blockData;

                    // XYZ POSITION
                    size_t pos = line.find(',');
                    blockData.blockPos.x = (float)std::stoi(line.substr(0, pos));
                    line.erase(0, pos+1);

                    pos = line.find(',');
                    blockData.blockPos.y = (float)std::stoi(line.substr(0, pos));
                    line.erase(0, pos+1);

                    pos = line.find(',');
                    blockData.blockPos.z = (float)std::stoi(line.substr(0, pos));
                    line.erase(0, pos+1);

                    // BLOCKID
                    pos = line.find(',');
                    blockData.blockType.blockID = (BLOCKID)std::stoi(line.substr(0, pos));
                    line.erase(0, pos+1);

                    // VARIANT
                    blockData.blockType.variantID = std::stoi(line.substr(0, std::string::npos));

                    // push to vector
                    structBlocks.push_back(blockData);
                }
            }
        }

        void EndStructureLoad() {
            lastStructure = loadedStructure;
            loadedStructure = STRUCTURE::NONE;
        }

        [[nodiscard]] STRUCTURE LoadedStructure() const {
            return loadedStructure;
        }

        [[nodiscard]] StructBlockData GetStructureBlock() {
            if (requestedBlocks == structBlocks.size()) return {};

            requestedBlocks++;
            if (requestedBlocks == structBlocks.size()) EndStructureLoad();

            return structBlocks[requestedBlocks-1];
        }
};

inline std::unique_ptr<StructureLoader> structureLoader {};



#endif //UNTITLED7_LOADSTRUCTURE_H
