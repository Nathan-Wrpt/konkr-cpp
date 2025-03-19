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
    int coins;
    bool alive;
public:
    Player(SDL_Color color);
    ~Player() = default;

    SDL_Color getColor() const { return color; }
    const std::vector<std::shared_ptr<Entity>>& getEntities() const { return entities; }

    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(std::shared_ptr<Entity> entity);
    int getCoins() const { return coins; }
    void addCoins(int amount) { coins += amount; }
    void removeCoins(int amount) { coins -= amount; }
    bool isAlive() const { return alive; }
    bool checkAlive(); 
};

#endif
