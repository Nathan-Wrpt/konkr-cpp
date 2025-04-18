#include "player.hpp"

Player::Player(SDL_Color color) : color(color) {
    coins = 10;
    townDestroyed = false;
    alive = true;
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
