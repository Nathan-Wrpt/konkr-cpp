#include "building.hpp"

// --- Town Class Implementation ---
Town::Town(Hex hex) 
    : Entity(hex, 1, "town", 0) {}

Town::~Town() {}

bool Town::move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor) {
    return false;
}

// --- Tower Class Implementation ---
Tower::Tower(Hex hex) 
    : Entity(hex, 2, "tower", 2) {}

Tower::~Tower() {}

bool Tower::move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor) {
    return false;
}