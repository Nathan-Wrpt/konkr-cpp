#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

#include <string>
#include <iostream>

#include "entity.hpp"

class Building : public Entity {
public:
    Building() : Entity(Hex(0, 0, 0), 1, "building") {} // Default constructor
    Building(Hex hex, int protection_level, std::string name, int upkeep = 0);
    virtual ~Building();
    bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
};

class Town : public Building {
public:
    Town() : Building(Hex(0, 0, 0), 1, "town") {} // Default constructor
    Town(Hex hex);
    virtual ~Town();
};
    
class Tower : public Building {
public:
    Tower() : Building(Hex(0, 0, 0), 2, "tower") {} // Default constructor
    Tower(Hex hex);
    virtual ~Tower();
};

#endif // BUILDINGS_HPP