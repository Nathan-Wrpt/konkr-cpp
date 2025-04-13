#include "entitymanager.hpp"

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