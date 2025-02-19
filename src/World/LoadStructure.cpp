//
// Created by cew05 on 19/02/2025.
//

#include "LoadStructure.h"

StructureLoader::StructureLoader() {
    std::filesystem::path dirPath = structuresDirRelPath;
    if (!std::filesystem::is_directory(dirPath)) {
        printf("Failed to Locate Structures Data Directory from %s\n", dirPath.string().c_str());
    }
}

StructureLoader::~StructureLoader() {
    structureBlocks.clear();
}


bool StructureLoader::validateStructureName(const std::string &_structureName) const {
    return structureBlocks.count(_structureName) > 0;
}

size_t StructureLoader::GetStructureSize(const std::string &_structureName) const {
    if (!validateStructureName(_structureName)) return 0;

    return structureBlocks.at(_structureName).size();
}

void StructureLoader::LoadStructures(const std::vector<std::string> &_structureFileNames) {
    for (const std::string& fileName : _structureFileNames) {
        LoadStructure(fileName);
    }
}

bool StructureLoader::LoadStructure(const std::string& _structureFileName) {
    // Validate StructureName
    std::filesystem::path dir = structuresDirRelPath;
    std::filesystem::path structData = _structureFileName;

    std::fstream structDataFile(dir / structData);
    if (!structDataFile.good()) {
        printf("Failed to locate file %s in dir %s", _structureFileName.c_str(), structuresDirRelPath.c_str());
        return false;
    }

    std::string structName = _structureFileName.substr(0, _structureFileName.find_last_of('.'));

    // Load block data from csv if structure is not already loaded
    if (validateStructureName(_structureFileName)) return false;

    StructBlocks structBlocks;

    std::string line;
    std::getline(structDataFile, line); // ignore first line as header

    while (std::getline(structDataFile, line)) {
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
        blockData.blockType.variantID = (GLbyte)std::stoi(line.substr(0, std::string::npos));

        // push to vector
        structBlocks.push_back(blockData);
    }

    // Add structdata to map
    structureBlocks[structName] = StructureData(_structureFileName, structBlocks);

    return true;
}


StructureData StructureLoader::GetStructureData(const std::string &_structureName) const {
    if (!validateStructureName(_structureName)) return {_structureName, {}};

    return structureBlocks.at(_structureName);
}