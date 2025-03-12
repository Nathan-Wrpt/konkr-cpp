#include "building.hpp"

// --- Building Class Implementation ---
Building::Building(Hex hex, std::string path_to_texture) 
    : hex(hex), path_to_texture(path_to_texture) {}

Building::~Building() {}

// --- Town Class Implementation ---
Town::Town(Hex hex) 
    : Building(hex, "icons/town.png") {}

Town::~Town() {}

// --- Tower Class Implementation ---
Tower::Tower(Hex hex) 
    : Building(hex, "icons/tower.png") {}

Tower::~Tower() {}