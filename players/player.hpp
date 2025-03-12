#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"

class Player {
private:
    SDL_Color color;
    std::vector<Entity*> entities;
    std::vector<Building*> buildings;
public:
    Player(SDL_Color color);
    ~Player();

    SDL_Color getColor() const { return color; }
    const std::vector<Entity*>& getEntities() const { return entities; }
    const std::vector<Building*>& getBuildings() const { return buildings; }

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void addBuilding(Building* building);
    void removeBuilding(Building* building);
};

#endif