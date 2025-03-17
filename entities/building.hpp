#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

#include <string>
#include <iostream>

#include "../core/grid.hpp"

class Building {
protected:
    Hex hex;
    std::string name;
public:
    Building(Hex hex, std::string name);
    virtual ~Building();

    // Getter for hex
    Hex getHex() const { return hex; }

    // Getter for texture path
    std::string getTexturePath() const { return name; }
};

class Town : public Building {
public:
    Town() : Building(Hex(0, 0, 0), "town") {} // Default constructor
    Town(Hex hex);
    virtual ~Town();
};
    
class Tower : public Building {
public:
    Tower() : Building(Hex(0, 0, 0), "tower") {} // Default constructor
    Tower(Hex hex);
    virtual ~Tower();
};

#endif // BUILDINGS_HPP