//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_TERRAINBLOCKS_H
#define UNTITLED7_TERRAINBLOCKS_H

#include "../BlockModels/Block.h"




class Stone : public Block {
    private:

    public:
        explicit Stone(GLbyte _variant);
};


class Dirt : public Block {
    private:


    public:
        explicit Dirt(GLbyte _variant);
};

class Grass : public Block {
    private:


    public:
        explicit Grass(GLbyte _variant);
};

class Sand : public Block {
    private:

    public:
        explicit Sand(GLbyte _variant);
};

class Water : public Block {
    private:

    public:
        explicit Water(GLbyte _variant);
};

class Air : public Block {
    private:

    public:
        explicit Air(GLbyte _variant);
};

class UnbreakableBlock : public Block
{
    public:
        explicit UnbreakableBlock(GLbyte _variant);
};
#endif //UNTITLED7_TERRAINBLOCKS_H
