#include "building.hpp"

// --- Building Class Implementation ---
Building::Building(Hex hex, int protection_level, std::string name, int upkeep) 
    : Entity(hex, protection_level, name, upkeep) {}

Building::~Building() {}

bool Building::move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor) {
    return false;
}

// --- Town Class Implementation ---
Town::Town(Hex hex) 
    : Building(hex, 1, "town", 0) {}

Town::~Town() {}

// --- Castle Class Implementation ---
Castle::Castle(Hex hex) 
    : Building(hex, 2, "castle", 2) {}

Castle::~Castle() {}

// --- BanditCamp Class Implementation ---
BanditCamp::BanditCamp(Hex hex) 
    : Building(hex, 1, "bandit_camp", 0) {
    coins = 0;
}

BanditCamp::~BanditCamp() {}

// --- Treasure Class Implementation ---
Treasure::Treasure(Hex hex, int value) 
    : Building(hex, 0, "treasure", 0), value(value) {}

Treasure::~Treasure() {}

// --- Forest Class Implementation ---
Forest::Forest(Hex hex) 
    : Building(hex, 0, "forest", 0) {}
    
Forest::~Forest() {}