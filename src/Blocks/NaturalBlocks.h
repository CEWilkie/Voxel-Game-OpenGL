//
// Created by cew05 on 23/07/2024.
//

#ifndef UNTITLED7_NATURALBLOCKS_H
#define UNTITLED7_NATURALBLOCKS_H

#include "Cube.h"

class Stone : public Cube {
    private:


    public:
        explicit Stone(glm::vec3 _position);
};

class Dirt : public Cube {
    private:


    public:
        explicit Dirt(glm::vec3 _position);
};

class Grass : public Cube {
    private:


    public:
        explicit Grass(glm::vec3 _position);
};


#endif //UNTITLED7_NATURALBLOCKS_H
