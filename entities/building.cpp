#include "building.hpp"

// --- Building Class Implementation ---
Building::Building(Hex hex, int protection_level, std::string name, int upkeep) 
    : Entity(hex, protection_level, name, 0) {}

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