#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "../core/grid.hpp"
#include "../entities/entity.hpp"

class EntityManager {
public:
    void addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits);
};

#endif