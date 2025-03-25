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
    
class Castle : public Building {
public:
    Castle();
    Castle(Hex hex);
    virtual ~Castle();
};

class BanditCamp : public Building {
public:
    BanditCamp();
    BanditCamp(Hex hex);
    virtual ~BanditCamp();
    // Getter for coins
    int getCoins() const { return coins; }
    // adder for coins
    void addCoins(int coins) { this->coins += coins; }
    void removeCoins(int coins) { this->coins -= coins; }

private:
    int coins;
};

#endif // BUILDINGS_HPP