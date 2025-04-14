#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

#include <queue>
#include <set>

#include "../entities/entitymanager.hpp"

class PlayerManager {
public:
    void checkIfHexConnectedToTown(Player& player, HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
private:
    void disconnectHex(Player& player, const Hex& hex, HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
    EntityManager entityManager;  
};

#endif // PLAYERMANAGER_HPP