#include "entitymanager.hpp"

void EntityManager::generateEntities(const std::vector<std::string>& entityMap, const std::vector<std::string>& asciiMap, HexagonalGrid& grid, std::vector<std::shared_ptr<Player>>& players, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, std::vector<std::shared_ptr<Treasure>>& treasures) {
    // Check if the entity map has the same number of characters on each row as the ASCII map
    if (entityMap.size() != asciiMap.size()) {
        std::cerr << "Error: Entity map and ASCII map have different number of rows." << std::endl;
        return;
    }
    for (size_t i = 0; i < entityMap.size(); ++i) {
        if (entityMap[i].size() != asciiMap[i].size()) {
            std::cerr << "Error: Entity map and ASCII map have different number of columns on row " << i << "." << std::endl;
            return;
        }
    }

    for (size_t row = 0; row < entityMap.size(); ++row) {
        const std::string& line = entityMap[row];
        for (size_t col = 0; col < line.size(); ++col) {
            char c = line[col];
            // Skip if no entity is present.
            if (c == '.') {
                continue;
            }

            // Convert (col, row) to hex coordinates.
            int q = static_cast<int>(col) - (static_cast<int>(row) / 2);
            int r = static_cast<int>(row);
            int s = -q - r;
            Hex hex(q, r, s);

            // Make sure the hex exists in the grid.
            if (!grid.hexExists(hex)) {
                continue;
            }

            if(c == 'B'){
                addBandit(hex, bandits);
                continue;
            }

            if(c == 'c'){
                addBanditCamp(hex, banditCamps);
                continue;
            }

            if(c == 't'){
                int treasureValue = std::rand() % 10 + 1;
                addTreasure(hex, treasureValue, treasures);
                continue;
            }

            SDL_Color hexColor = grid.getHexColors().at(hex);

            // Find the player whose color matches with hex.
            std::shared_ptr<Player> playerForEntity = nullptr;
            for (auto& player : players) {
                if (player->getColor() == hexColor) {
                    playerForEntity = player;
                    break;
                }
            }

            if (!playerForEntity) {
                continue;
            }

            // Add entity
            if (c == 'T') {
                playerForEntity->addEntity(std::make_shared<Town>(hex));
                playerForEntity->getEntities().back()->setMoved(true);
            } else if (c == 'V') {
                playerForEntity->addEntity(std::make_shared<Villager>(hex));
            } else if (c == 'C') {
                playerForEntity->addEntity(std::make_shared<Castle>(hex));
                playerForEntity->getEntities().back()->setMoved(true);
            }
            else if (c == 'P') {
                playerForEntity->addEntity(std::make_shared<Pikeman>(hex));
            } else if (c == 'K') {
                playerForEntity->addEntity(std::make_shared<Knight>(hex));
            } else if (c == 'H') {
                playerForEntity->addEntity(std::make_shared<Hero>(hex));
            }
        }
    }
}

void EntityManager::upgradeEntity(const Hex& hex, std::vector<std::shared_ptr<Player>>& players) {
    for (auto& player : players) {
        for (auto& entity : player->getEntities()) {
            if (entity->getHex() == hex) {
                bool hasmoved = entity->hasMoved();
                if (entity->getName() == "villager") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Pikeman>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                } else if (entity->getName() == "pikeman") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Knight>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                } else if (entity->getName() == "knight") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Hero>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                }
            }
        }
    }
}

bool EntityManager::entityOnHex(const Hex& hex, const std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players) const {
    for(const auto& bandit : bandits) {
        if (bandit->getHex() == hex) {
            return true;
        }
    }
    for(const auto& banditcamp : banditCamps) {
        if (banditcamp->getHex() == hex) {
            return true;
        }
    }
    for (const auto& player : players) {
        for (const auto& entity : player->getEntities()) {
            if (entity->getHex() == hex) {
                return true;
            }
        }
    }
    for (const auto& treasure : treasures) {
        if (treasure->getHex() == hex) {
            return true;
        }
    }
    for (const auto& devil : devils) {
        if (devil->getHex() == hex) {
            return true;
        }
    }
    return false;
}

void EntityManager::manageBandits(HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, std::vector<std::shared_ptr<Treasure>>& treasures, std::vector<std::shared_ptr<Devil>>& devils, std::vector<std::shared_ptr<Player>>& players) {
    // Directions possibles sur une grille hexagonale (pointy-top orientation)
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };

    for (const auto& bandit : bandits) {
        bool moved = false;
        int maxAttempts = 10; // Limite le nombre de tentatives
        int attempts = 0;

        // Essayer de trouver une position valide
        while (!moved && attempts < maxAttempts) {
            // Choisir une direction aléatoire
            Hex direction = directions[std::rand() % directions.size()];

            // Calculer la nouvelle position
            Hex newHex = bandit->getHex().add(direction);

            // Check if there's a treasure on the new hex
            bool treasureonHex = false;
            for(auto& treasure : treasures) {
                if(treasure->getHex() == newHex) {
                    treasureonHex = true;
                    break;
                }
            }

            // CHECK IF DEVIL ON HEX
            bool devilonHex = false;
            for(auto& devil : devils) {
                if(devil->getHex() == newHex) {
                    devilonHex = true;
                    break;
                }
            }

            // Vérifier si la nouvelle position est valide
            if (grid.hexExists(newHex) && !entityOnHex(newHex, bandits, banditCamps, treasures, devils, players) && !treasureonHex && !devilonHex) {
                bandit->moveBandit(grid, newHex);
                moved = true;
            } else {
                attempts++;
            }
        }
        Hex banditHex = bandit->getHex();
        SDL_Color hexColor = grid.getHexColors().at(banditHex);
        for(auto& player : players) {
            if(hexColor == player->getColor()) {
                // steal a coin from the player
                player->removeCoins(1);

                // locate the nearest bandit camp
                int minDistance = 1000;
                std::shared_ptr<BanditCamp> nearestBanditCamp = nullptr;
                for(auto& banditcamp : banditCamps) {
                    int distance = banditHex.distance(banditcamp->getHex());
                    if(distance < minDistance) {
                        minDistance = distance;
                        nearestBanditCamp = banditcamp;
                    }
                }
                if(nearestBanditCamp) {
                    // give the stolen coin to the nearest bandit camp if it exists
                    nearestBanditCamp->addCoins(1);
                }
            }
        }
    }
    int banditCost = 5;
    for(auto& banditcamp : banditCamps) {
        if(banditcamp->getCoins() >= banditCost){
            // spawn a bandit
            int maxAttempts = 100;
            int attempts = 0;
            bool placed = false;
            while (!placed && attempts < maxAttempts) {
                Hex direction = directions[std::rand() % directions.size()];

                Hex newHex = banditcamp->getHex().add(direction);

                if (grid.hexExists(newHex) && !entityOnHex(newHex, bandits, banditCamps, treasures, devils, players)) {
                    addBandit(newHex, bandits);
                    banditcamp->removeCoins(banditCost);
                    placed = true;
                } else {
                    attempts++;
                }
            }
        }
    }
}

void EntityManager::addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits) {
    bandits.push_back(std::make_shared<Bandit>(hex));
}

void EntityManager::addBanditCamp(const Hex& hex, std::vector<std::shared_ptr<BanditCamp>>& banditCamps) {
    banditCamps.push_back(std::make_shared<BanditCamp>(hex));
}

void EntityManager::addTreasure(const Hex& hex, int value, std::vector<std::shared_ptr<Treasure>>& treasures) {
    treasures.push_back(std::make_shared<Treasure>(hex, value));
}

void EntityManager::addDevil(const Hex& hex, std::vector<std::shared_ptr<Devil>>& devils) {
    devils.push_back(std::make_shared<Devil>(hex));
}

bool EntityManager::isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const {
    // Define the directions to the six neighbors
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };

    for (auto& player : players) {
        if (player->getColor() == currentPlayer.getColor()) {
            continue;
        }

        for (const auto& direction : directions) {
            Hex neighbor = hex.add(direction);
            if (grid.hexExists(neighbor)) {
                for (const auto& entity : player->getEntities()) {
                    if (entity->getHex() == neighbor &&
                        entity->getProtectionLevel() >= currentLevel &&
                        grid.getHexColors().at(hex) == player->getColor()) {
                        return true;
                    }
                }
            }
        }

        for (const auto& entity : player->getEntities()) {
            if (entity->getHex() == hex &&
                grid.getHexColors().at(hex) == player->getColor()) {
                if (entity->getProtectionLevel() >= currentLevel) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    for (const auto& banditcamp : banditCamps) {
        if (banditcamp->getHex() == hex && banditcamp->getProtectionLevel() >= currentLevel) {
            return true;
        }
    }
    for (const auto& devil : devils) {
        if (devil->getHex() == hex && devil->getProtectionLevel() >= currentLevel) {
            return true;
        }
    }
    return false;
}

bool EntityManager::HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const {
    // Check if the hex exists in the grid
    if (!grid.hexExists(targetHex)) {
        return false;
    }

    // Get the current player
    auto& currentPlayer = players[playerTurn];

    // Check if hex is adjacent to the owner's territory
    SDL_Color ownerColor = currentPlayer->getColor();
    if (!grid.hasNeighborWithColor(targetHex, ownerColor)) {
        return false;
    }

    // Check if the hex is not on the player's territory
    if (grid.getHexColors().at(targetHex) == currentPlayer->getColor()) {
        return false;
    }

    // Check if the hex is surrounded by stronger entities from other players
    if (isSurroundedByOtherPlayerEntities(targetHex, *currentPlayer, entity->getProtectionLevel(), grid, players, banditCamps, devils)) {
        return false;
    }

    return true;
}