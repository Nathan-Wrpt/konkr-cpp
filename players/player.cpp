#include "player.hpp"

Player::Player(SDL_Color color) : color(color) {
    coins = 12;
}

void Player::addEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}

void Player::removeEntity(std::shared_ptr<Entity> entity) {
    // Using STL algorithm to find and remove the entity
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}

void Player::addBuilding(std::shared_ptr<Building> building) {
    buildings.push_back(building);
}

void Player::removeBuilding(std::shared_ptr<Building> building) {
    auto it = std::find(buildings.begin(), buildings.end(), building);
    if (it != buildings.end()) {
        buildings.erase(it);
    }
}
