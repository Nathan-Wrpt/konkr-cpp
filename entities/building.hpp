#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

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

class Treasure : public Building {
public:
    Treasure(int value);
    Treasure(Hex hex, int value);
    virtual ~Treasure();
    int getValue() const { return value; }
private:
    int value;
};

class Forest : public Building {
public:
    Forest();
    Forest(Hex hex);
    virtual ~Forest();
};

#endif // BUILDINGS_HPP