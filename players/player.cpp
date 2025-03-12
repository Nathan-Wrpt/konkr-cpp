#include "player.hpp"

Player::Player(SDL_Color color) : color(color) {}

Player::~Player() {
    for (Entity* entity : entities) {
        delete entity;
    }
    entities.clear();

    for (Building* building : buildings) {
        delete building;
    }
    buildings.clear();
}

void Player::addEntity(Entity* entity) {
    entities.push_back(entity);
}

void Player::removeEntity(Entity* entity) {
    for (size_t i = 0; i < entities.size(); ++i) {
        if (entities[i] == entity) {
            entities.erase(entities.begin() + i);
            delete entity;
            return;
        }
    }
}

void Player::addBuilding(Building* building) {
    buildings.push_back(building);
}

void Player::removeBuilding(Building* building) {
    for (size_t i = 0; i < buildings.size(); ++i) {
        if (buildings[i] == building) {
            buildings.erase(buildings.begin() + i);
            delete building;
            return;
        }
    }
}