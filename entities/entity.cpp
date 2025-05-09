#include "entity.hpp"

// --- Entity Class Implementation ---

Entity::Entity(Hex hex, int protection_level, std::string name, int upkeep) :
    hex(hex),
    protection_level(protection_level),
    moved(false),
    name(name),
    upkeep(upkeep),
    yOffset(0.0f),
    jumpSpeed(0.5f),
    jumping(false) {}


Entity::~Entity() {}

bool Entity::move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor) {
    if (grid.hexExists(target)) {
        auto it = grid.getHexColors().find(target);
        if (it != grid.getHexColors().end()) {
            const SDL_Color& targetColor = it->second;

            // Check if the target color matches the owner's color
            if (targetColor == ownerColor) {
                // Move the entity to the target hex
                hex = target;
                return true;
            } else {
                // Check if the target is a valid position on the grid
                if (grid.hasNeighborWithColor(target, ownerColor)) {
                    hex = target;

                    // Change the color of the hex to the owner's color
                    grid.setHexColor(target, ownerColor);
                    return true;
                }
            }
        }
    }
    return false;
}



// --- Bandit Class Implementation ---

Bandit::Bandit(Hex hex) 
    : Entity(hex, 0, "bandit", 0) {}

Bandit::~Bandit() {}

bool Bandit::moveBandit(HexagonalGrid& grid, Hex target) {
    if(grid.hexExists(target)) {
        hex = target;
        return true;
    }
    return false;
}

// --- Villager Class Implementation ---

Villager::Villager(Hex hex) 
    : Entity(hex, 1, "villager", 2) {}

Villager::~Villager() {}

// --- Pikeman Class Implementation ---

Pikeman::Pikeman(Hex hex) 
    : Entity(hex, 2, "pikeman", 6) {}

Pikeman::~Pikeman() {}

// --- Knight Class Implementation ---
Knight::Knight(Hex hex) 
    : Entity(hex, 3, "knight", 18) {}

Knight::~Knight() {}

// --- Hero Class Implementation ---
Hero::Hero(Hex hex) 
    : Entity(hex, 4, "hero", 54) {}

Hero::~Hero() {}

// --- Devil Class Implementation ---

Devil::Devil(Hex hex) 
    : Entity(hex, 2, "devil", 100) {}
Devil::~Devil() {}