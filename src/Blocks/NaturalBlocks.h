//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_NATURALBLOCKS_H
#define UNTITLED7_NATURALBLOCKS_H

#include "../BlockModels/Block.h"




class Stone : public Block {
    private:

    public:
        explicit Stone(int _variant);
};


class Dirt : public Block {
    private:


    public:
        explicit Dirt(int _variant);
};

class Grass : public Block {
    private:


    public:
        explicit Grass(int _variant);
};

class Water : public Block {
    private:

    public:
        explicit Water(int _variant);
};

class Air : public Block {
    private:

    public:
        explicit Air(int _variant);
};


#endif //UNTITLED7_NATURALBLOCKS_H
