#include "building.hpp"

// --- Building Class Implementation ---
Building::Building(Hex hex, std::string name) 
    : hex(hex), name(name) {}

Building::~Building() {}

// --- Town Class Implementation ---
Town::Town(Hex hex) 
    : Building(hex, "town") {}

Town::~Town() {}

// --- Tower Class Implementation ---
Tower::Tower(Hex hex) 
    : Building(hex, "tower") {}

Tower::~Tower() {}