#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

#include <string>
#include <iostream>

#include "entity.hpp"

class Building : public Entity {
public:
    Building();
    Building(Hex hex, int protection_level, std::string name, int upkeep = 0);
    virtual ~Building();
    bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
};

class Town : public Building {
public:
    Town();
    Town(Hex hex);
    virtual ~Town();
};
    
class Tower : public Building {
public:
    Tower();
    Tower(Hex hex);
    virtual ~Tower();
};

#endif // BUILDINGS_HPP