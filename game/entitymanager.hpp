#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"

class EntityManager {
public:
    void addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits);
    void addBanditCamp(const Hex& hex, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
    void addTreasure(const Hex& hex, int value, std::vector<std::shared_ptr<Treasure>>& treasures);
    void addDevil(const Hex& hex, std::vector<std::shared_ptr<Devil>>& devils);
};

#endif