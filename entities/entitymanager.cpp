#include "entitymanager.hpp"

void EntityManager::addEntityToPlayer(char entityType, const Hex& hex, std::shared_ptr<Player>& player) {
    std::shared_ptr<Entity> entity;
    switch (entityType) {
        case 'T':
            entity = std::make_shared<Town>(hex);
            entity->setMoved(true);
            break;
        case 'V':
            entity = std::make_shared<Villager>(hex);
            break;
        case 'C':
            entity = std::make_shared<Castle>(hex);
            entity->setMoved(true);
            break;
        case 'P':
            entity = std::make_shared<Pikeman>(hex);
            break;
        case 'K':
            entity = std::make_shared<Knight>(hex);
            break;
        case 'H':
            entity = std::make_shared<Hero>(hex);
            break;
        default:
            return;
    }
    player->addEntity(entity);
}

void EntityManager::generateEntities(const std::vector<std::string>& entityMap, const std::vector<std::string>& asciiMap, HexagonalGrid& grid, GameEntities& gameEntities) {
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

            switch (c) {
                case 'B':
                    addBandit(hex, gameEntities.bandits);
                    break;
                case 'c':
                    addBanditCamp(hex, gameEntities.banditCamps);
                    break;
                case 't': {
                    int treasureValue = std::rand() % 10 + 1;
                    addTreasure(hex, treasureValue, gameEntities.treasures);
                    break;
                }
                case 'f': {
                    addForest(hex, gameEntities.forests);
                    break;
                }

                default: {
                    SDL_Color hexColor = grid.getHexColors().at(hex);

                    // Find the player whose color matches with hex.
                    std::shared_ptr<Player> playerForEntity = nullptr;
                    for (auto& player : gameEntities.players) {
                        if (player->getColor() == hexColor) {
                            playerForEntity = player;
                            break;
                        }
                    }
                    
                    // Add the entity to the player.
                    if (playerForEntity) {
                        addEntityToPlayer(c, hex, playerForEntity);
                    }
                    break;
                }
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

bool EntityManager::entityOnHex(const Hex& hex, const GameEntities& gameEntities) const {
    for(const auto& bandit : gameEntities.bandits) {
        if (bandit->getHex() == hex) {
            return true;
        }
    }
    for(const auto& banditcamp : gameEntities.banditCamps) {
        if (banditcamp->getHex() == hex) {
            return true;
        }
    }
    for (const auto& player : gameEntities.players) {
        for (const auto& entity : player->getEntities()) {
            if (entity->getHex() == hex) {
                return true;
            }
        }
    }
    for (const auto& treasure : gameEntities.treasures) {
        if (treasure->getHex() == hex) {
            return true;
        }
    }
    for (const auto& devil : gameEntities.devils) {
        if (devil->getHex() == hex) {
            return true;
        }
    }
    for (const auto& forest : gameEntities.forests) {
        if (forest->getHex() == hex) {
            return true;
        }
    }
    return false;
}

void EntityManager::moveBanditToNewPosition(HexagonalGrid& grid, std::shared_ptr<Bandit>& bandit, const GameEntities& gameEntities) {
    bool moved = false;
    int maxAttempts = 10;
    int attempts = 0;

    while (!moved && attempts < maxAttempts) {
        Hex direction = directions[std::rand() % directions.size()];
        Hex newHex = bandit->getHex().add(direction);

        bool treasureOnHex = false;
        for (const auto& treasure : gameEntities.treasures) {
            if (treasure->getHex() == newHex) {
                treasureOnHex = true;
                break;
            }
        }

        bool devilOnHex = false;
        for (const auto& devil : gameEntities.devils) {
            if (devil->getHex() == newHex) {
                devilOnHex = true;
                break;
            }
        }

        bool forestOnHex = false;
        for (const auto& forest : gameEntities.forests) {
            if (forest->getHex() == newHex) {
                forestOnHex = true;
                break;
            }
        }

        if (grid.hexExists(newHex) && !entityOnHex(newHex, gameEntities) && !treasureOnHex && !devilOnHex && !forestOnHex) {
            bandit->moveBandit(grid, newHex);
            moved = true;
        } else {
            attempts++;
        }
    }
}

void EntityManager::stealCoinFromPlayer(HexagonalGrid& grid, const std::shared_ptr<Bandit>& bandit, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Player>>& players) {
    Hex banditHex = bandit->getHex();
    SDL_Color hexColor = grid.getHexColors().at(banditHex);
    for (const auto& player : players) {
        if (hexColor == player->getColor()) {
            player->removeCoins(1);

            std::shared_ptr<BanditCamp> nearestBanditCamp = nullptr;
            int minDistance = std::numeric_limits<int>::max();
            for (const auto& banditCamp : banditCamps) {
                int distance = banditHex.distance(banditCamp->getHex());
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestBanditCamp = banditCamp;
                }
            }
            if (nearestBanditCamp) {
                nearestBanditCamp->addCoins(1);
            }
        }
    }
}

void EntityManager::spawnBanditFromCamp(HexagonalGrid& grid, std::shared_ptr<BanditCamp>& banditCamp, std::vector<std::shared_ptr<Bandit>>& bandits, const GameEntities& gameEntities) {
    int maxAttempts = 100;
    int attempts = 0;
    bool placed = false;
    while (!placed && attempts < maxAttempts) {
        Hex direction = directions[std::rand() % directions.size()];
        Hex newHex = banditCamp->getHex().add(direction);

        if (grid.hexExists(newHex) && !entityOnHex(newHex, gameEntities)) {
            addBandit(newHex, bandits);
            banditCamp->removeCoins(5);
            placed = true;
        } else {
            attempts++;
        }
    }
}

void EntityManager::manageBandits(HexagonalGrid& grid, GameEntities& gameEntities) {
    for (auto& bandit : gameEntities.bandits) {
        moveBanditToNewPosition(grid, bandit, gameEntities);
        stealCoinFromPlayer(grid, bandit, gameEntities.banditCamps, gameEntities.players);
    }

    int banditCost = 5;
    for (auto& banditCamp : gameEntities.banditCamps) {
        if (banditCamp->getCoins() >= banditCost) {
            spawnBanditFromCamp(grid, banditCamp, gameEntities.bandits, gameEntities);
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

void EntityManager::addForest(const Hex& hex, std::vector<std::shared_ptr<Forest>>& forests) {
    forests.push_back(std::make_shared<Forest>(hex));
}

bool EntityManager::isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel, const HexagonalGrid& grid, const GameEntities& gameEntities) const {
    for (auto& player : gameEntities.players) {
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
    for (const auto& banditcamp : gameEntities.banditCamps) {
        if (banditcamp->getHex() == hex && banditcamp->getProtectionLevel() >= currentLevel) {
            return true;
        }
    }
    for (const auto& devil : gameEntities.devils) {
        if (devil->getHex() == hex && devil->getProtectionLevel() >= currentLevel) {
            return true;
        }
    }
    for (const auto& forest : gameEntities.forests) {
        if (forest->getHex() == hex && forest->getProtectionLevel() >= currentLevel) {
            return true;
        }
    }
    return false;
}

bool EntityManager::HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex, const HexagonalGrid& grid, size_t playerTurn, const GameEntities& gameEntities) const {
    // Check if the hex exists in the grid
    if (!grid.hexExists(targetHex)) {
        return false;
    }

    // Get the current player
    auto& currentPlayer = gameEntities.players[playerTurn];

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
    if (isSurroundedByOtherPlayerEntities(targetHex, *currentPlayer, entity->getProtectionLevel(), grid, gameEntities)) {
        return false;
    }

    return true;
}

Hex EntityManager::randomfreeHex(const HexagonalGrid& grid, const GameEntities& gameEntities) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, grid.getHexes().size() - 1);
    int randomIndex = distrib(gen);
    Hex randomHex = grid.getHexes()[randomIndex];
    SDL_Color hexColor = grid.getHexColors().at(randomHex);
    std::vector<SDL_Color> playerColors;
    for(auto& player : gameEntities.players) {
        playerColors.push_back(player->getColor());
    }
    // check that the Hex is not occupated and not owned by a player
    int maxAttempts = 100;
    int attempts = 0;
    while(entityOnHex(randomHex, gameEntities) || (std::find(playerColors.begin(), playerColors.end(), hexColor) != playerColors.end())) {
        randomIndex = distrib(gen);
        randomHex = grid.getHexes()[randomIndex];
        hexColor = grid.getHexColors().at(randomHex);
        attempts++;
        if(attempts >= maxAttempts) {
            return Hex(-1000, 0, 1000);
        }
    }
    return randomHex;
}