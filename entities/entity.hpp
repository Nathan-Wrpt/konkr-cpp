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

    // Getter for hex
    Hex getHex() const { return hex; }

    // Getter for texture path
    std::string getTexturePath() const { return path_to_texture; }

    virtual bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);

};

class Bandit : public Entity {
public:
    Bandit() : Entity(Hex(0, 0, 0), 1, "icons/bandit.png") {} // Default constructor
    Bandit(Hex hex);
    virtual ~Bandit();        
    bool move(HexagonalGrid& grid, const SDL_Color& ownerColor = {255, 255, 255, 255});
    };
        
    
class Villager : public Entity {
public:
    Villager() : Entity(Hex(0, 0, 0), 1, "icons/villager.png") {} // Default constructor
    Villager(Hex hex);
    virtual ~Villager();
};

#endif // ENTITY_HPP