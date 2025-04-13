#include "entitymanager.hpp"

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