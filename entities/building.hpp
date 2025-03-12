#ifndef BUILDINGS_HPP
#define BUILDINGS_HPP

#include <string>
#include <iostream>

#include "../core/grid.hpp"

class Building {
    protected:
        Hex hex;
        std::string path_to_texture;
    public:
        Building(Hex hex, std::string path_to_texture);
        virtual ~Building();
    
        // Getter for hex
        Hex getHex() const { return hex; }
    
        // Getter for texture path
        std::string getTexturePath() const { return path_to_texture; }
    };

class Town : public Building {
    public:
        Town() : Building(Hex(0, 0, 0), "icons/town.png") {} // Default constructor
        Town(Hex hex);
        virtual ~Town();
    };
    
    class Tower : public Building {
    public:
        Tower() : Building(Hex(0, 0, 0), "icons/tower.png") {} // Default constructor
        Tower(Hex hex);
        virtual ~Tower();
    };

#endif // BUILDINGS_HPP