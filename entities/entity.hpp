#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <iostream>

#include "../core/grid.hpp"

class Entity {
protected:
    Hex hex;
    int protection_level;
    std::string path_to_texture;
public:
    Entity(Hex hex, int protection_level, std::string path_to_texture);
    virtual ~Entity();
};

class Bandit : public Entity {
public:
    Bandit(Hex hex);
    virtual ~Bandit();
    // bool move_is_valid(Hex hex);
    void move(HexagonalGrid& grid);
};

class Villager : public Entity {
public:
    Villager(Hex hex);
    virtual ~Villager();
    void move(HexagonalGrid& grid, Hex target);
    };

#endif // ENTITY_HPP