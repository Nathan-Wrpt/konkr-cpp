#include "entitymanager.hpp"

void EntityManager::addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits) {
    bandits.push_back(std::make_shared<Bandit>(hex));
}