//
// Created by cew05 on 19/08/2024.
//

#ifndef UNTITLED7_NATURALBLOCKS_H
#define UNTITLED7_NATURALBLOCKS_H

#include "../BlockModels/Block.h"

class Leaves : public Block {
    private:

    public:
        explicit Leaves(int _variant);
};

class Wood : public Block {
    private:

    public:
        explicit Wood(int _variant);
};

class GrassPlant : public Block {
    private:

    public:
        explicit GrassPlant(int _variant);
};

#endif //UNTITLED7_NATURALBLOCKS_H
