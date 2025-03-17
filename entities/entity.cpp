#include "entity.hpp"

// --- Entity Class Implementation ---

Entity::Entity(Hex hex, int protection_level, std::string name, int upkeep) 
    : hex(hex), protection_level(protection_level), name(name), upkeep(upkeep) {
        moved = false;
    }

Entity::~Entity() {}

bool Entity::move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor) {
    if (grid.hexExists(target)) {
        // Check if the target is the same color as the ownerColor
        auto it = grid.getHexColors().find(target);
        if (it != grid.getHexColors().end()) {
            const SDL_Color& targetColor = it->second;

            // Check if the target color matches the owner's color
            if (targetColor == ownerColor) {
                // Move the entity to the target hex
                hex = target;
                // Return false because the entity can be moved again
                return false;
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
    std::cout << "Error: Target position is not valid." << std::endl;
    return false;
}



// --- Bandit Class Implementation ---

Bandit::Bandit(Hex hex) 
    : Entity(hex, 0, "bandit", 0) {}

Bandit::~Bandit() {}

bool Bandit::move(HexagonalGrid& grid, Hex target) {
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