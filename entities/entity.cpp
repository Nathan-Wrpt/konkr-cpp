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

bool Bandit::move(HexagonalGrid& grid, const SDL_Color& ownerColor) {
    // Directions possibles sur une grille hexagonale (pointy-top orientation)
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };

    bool moved = false;
    int maxAttempts = 10; // Limite le nombre de tentatives
    int attempts = 0;

    // Essayer de trouver une position valide
    while (!moved && attempts < maxAttempts) {
        // Choisir une direction aléatoire
        Hex direction = directions[std::rand() % directions.size()];

        // Calculer la nouvelle position
        Hex newHex = hex.add(direction);

        // Vérifier si la nouvelle position est valide
        if (grid.hexExists(newHex)) {
            // Mettre à jour la position du Bandit
            hex = newHex;            
            moved = true;
        }

        attempts++;
    }

    // Si aucune position valide n'est trouvée, le Bandit reste en place
    if (!moved) {
        std::cout << "No valid position found for Bandit" << std::endl;
        return false;
    }
    return true;
}

// --- Villager Class Implementation ---

Villager::Villager(Hex hex) 
    : Entity(hex, 1, "villager", 2) {}

Villager::~Villager() {}

// --- Pikeman Class Implementation ---

Pikeman::Pikeman(Hex hex) 
    : Entity(hex, 2, "pikeman", 6) {}

Pikeman::~Pikeman() {}