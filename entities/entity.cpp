#include "entity.hpp"

// --- Entity Class Implementation ---

Entity::Entity(Hex hex, int protection_level, std::string path_to_texture) 
    : hex(hex), protection_level(protection_level), path_to_texture(path_to_texture) {}

Entity::~Entity() {}

// --- Bandit Class Implementation ---

Bandit::Bandit(Hex hex) 
    : Entity(hex, 1, "icons/bandit.png") {}

Bandit::~Bandit() {}

void Bandit::move(HexagonalGrid& grid) {
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
    }
}

// --- Villager Class Implementation ---

Villager::Villager(Hex hex) 
    : Entity(hex, 1, "icons/villager.png") {}

Villager::~Villager() {}

void Villager::move(HexagonalGrid& grid, Hex target) {
    // Vérifier si la cible est une position valide sur la grille
    if (grid.hexExists(target)) {
        hex = target;
    } else {
        std::cout << "Error: Target position is not valid." << std::endl;
    }
}