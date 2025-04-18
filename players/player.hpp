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
    bool townDestroyed;
    bool alive;

public:
    Player(SDL_Color color);
    ~Player() = default;

    // Copy constructor
    Player(const Player& other) : color(other.color), coins(other.coins), townDestroyed(other.townDestroyed), alive(other.alive) {
        entities.reserve(other.entities.size());
        for (const auto& entity : other.entities) {
            if (entity->getName() == "villager") {
                entities.push_back(std::make_shared<Villager>(*dynamic_cast<Villager*>(entity.get())));
            } else if (entity->getName() == "pikeman") {
                entities.push_back(std::make_shared<Pikeman>(*dynamic_cast<Pikeman*>(entity.get())));
            } else if (entity->getName() == "knight") {
                entities.push_back(std::make_shared<Knight>(*dynamic_cast<Knight*>(entity.get())));
            } else if (entity->getName() == "hero") {
                entities.push_back(std::make_shared<Hero>(*dynamic_cast<Hero*>(entity.get())));
            } else if (entity->getName() == "town") {
                entities.push_back(std::make_shared<Town>(*dynamic_cast<Town*>(entity.get())));
            } else if (entity->getName() == "castle") {
                entities.push_back(std::make_shared<Castle>(*dynamic_cast<Castle*>(entity.get())));
            } 
        }
    }
    // Assignment operator
    Player& operator=(const Player& other) {
        if (this != &other) {
            color = other.color;
            coins = other.coins;
            townDestroyed = other.townDestroyed;
            alive = other.alive;

            // Clear existing entities
            entities.clear();
            entities.reserve(other.entities.size());

            // Deep copy entities
             for (const auto& entity : other.entities) {
                if (entity->getName() == "Bandit") {
                    entities.push_back(std::make_shared<Bandit>(*dynamic_cast<Bandit*>(entity.get())));
                } else if (entity->getName() == "Villager") {
                    entities.push_back(std::make_shared<Villager>(*dynamic_cast<Villager*>(entity.get())));
                } else if (entity->getName() == "Pikeman") {
                    entities.push_back(std::make_shared<Pikeman>(*dynamic_cast<Pikeman*>(entity.get())));
                } else if (entity->getName() == "Knight") {
                    entities.push_back(std::make_shared<Knight>(*dynamic_cast<Knight*>(entity.get())));
                } else if (entity->getName() == "Hero") {
                    entities.push_back(std::make_shared<Hero>(*dynamic_cast<Hero*>(entity.get())));
                } else if (entity->getName() == "BanditCamp") {
                    entities.push_back(std::make_shared<BanditCamp>(*dynamic_cast<BanditCamp*>(entity.get())));
                } else {
                    // Handle other entity types appropriately (e.g., Buildings)
                    entities.push_back(entity->clone()); // Use the clone method to create a copy
                }
            }
        }
        return *this;
    }

    SDL_Color getColor() const { return color; }
    void setColor(SDL_Color color) { this->color = color; }
    const std::vector<std::shared_ptr<Entity>>& getEntities() const { return entities; }

    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(std::shared_ptr<Entity> entity);
    int getCoins() const { return coins; }
    void addCoins(int amount) { coins += amount; }
    void removeCoins(int amount) { coins -= amount; }
    bool isTownDestroyed() const { return townDestroyed; }
    void setTownDestroyed(bool destroyed) { townDestroyed = destroyed; }
    bool isAlive() const { return alive; }
    void setAlive(bool alive) { this->alive = alive; }
};

#endif
