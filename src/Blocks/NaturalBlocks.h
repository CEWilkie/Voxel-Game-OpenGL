//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_NATURALBLOCKS_H
#define UNTITLED7_NATURALBLOCKS_H

#include "Block.h"

class Stone : public Block {
    private:


    public:
        explicit Stone(glm::vec3 _position);
};

class Dirt : public Block {
    private:


    public:
        explicit Dirt(glm::vec3 _position);
};

class Grass : public Block {
    private:


    public:
        explicit Grass(glm::vec3 _position);
};

class Water : public Block {
    private:

    public:
        explicit Water(glm::vec3 _position);
};

class Air : public Block {
    private:

    public:
        explicit Air(glm::vec3 _position);
};


#endif //UNTITLED7_NATURALBLOCKS_H
