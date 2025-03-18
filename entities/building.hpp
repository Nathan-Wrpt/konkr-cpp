#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

#include <string>
#include <iostream>

#include "entity.hpp"

class Town : public Entity {
public:
    Town() : Entity(Hex(0, 0, 0), 1, "town") {} // Default constructor
    Town(Hex hex);
    virtual ~Town();
    bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
};
    
class Tower : public Entity {
public:
    Tower() : Entity(Hex(0, 0, 0), 2, "tower") {} // Default constructor
    Tower(Hex hex);
    virtual ~Tower();
    bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
};

#endif // BUILDINGS_HPP