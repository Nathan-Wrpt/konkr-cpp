#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <SDL2/SDL.h>
#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"

class Player {
private:
    SDL_Color color;
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Building>> buildings;
    int coins;
    bool alive;
public:
    Player(SDL_Color color);
    ~Player() = default;

    SDL_Color getColor() const { return color; }
    const std::vector<std::shared_ptr<Entity>>& getEntities() const { return entities; }
    const std::vector<std::shared_ptr<Building>>& getBuildings() const { return buildings; }

    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(std::shared_ptr<Entity> entity);
    void addBuilding(std::shared_ptr<Building> building);
    void removeBuilding(std::shared_ptr<Building> building);
    int getCoins() const { return coins; }
    void addCoins(int amount) { coins += amount; }
    void removeCoins(int amount) { coins -= amount; }
    bool isAlive() const { return alive; }
};

#endif
