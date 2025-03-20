#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <iostream>

#include "../core/grid.hpp"

class Entity {
protected:
    Hex hex;
    int protection_level;
    bool moved;
    std::string name;
    int upkeep;
public:
    Entity(Hex hex, int protection_level, std::string name, int upkeep = 0);
    virtual ~Entity();

    // Getter for hex
    Hex getHex() const { return hex; }

    // Getter for name
    std::string getName() const { return name; }

    // Getter for protection level
    int getProtectionLevel() const { return protection_level; }

    // Getter for moved
    bool hasMoved() const { return moved; }

    // Setter for moved
    void setMoved(bool moved) { this->moved = moved; }
    
    virtual bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);

    // Getter for upkeep
    int getUpkeep() const { return upkeep; }
};

class Bandit : public Entity {
public:
    Bandit();
    Bandit(Hex hex);
    virtual ~Bandit();        
    bool move(HexagonalGrid& grid, Hex target);
    };
        
    
class Villager : public Entity {
public:
    Villager();
    Villager(Hex hex);
    virtual ~Villager();
};

class Pikeman : public Entity {
public:
    Pikeman();
    Pikeman(Hex hex);
    virtual ~Pikeman();
};

class Knight : public Entity {
    public:
        Knight();
        Knight(Hex hex);
        virtual ~Knight();
    };
    
class Hero : public Entity {
    public:
        Hero();
        Hero(Hex hex);
        virtual ~Hero();
    };

#endif // ENTITY_HPP