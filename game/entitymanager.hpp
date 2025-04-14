#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"
#include "../players/player.hpp"

class EntityManager {
public:
    bool entityOnHex(const Hex& hex, const std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players) const;
    void manageBandits(HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, std::vector<std::shared_ptr<Treasure>>& treasures, std::vector<std::shared_ptr<Devil>>& devils, std::vector<std::shared_ptr<Player>>& players);
    void addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits);
    void addBanditCamp(const Hex& hex, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
    void addTreasure(const Hex& hex, int value, std::vector<std::shared_ptr<Treasure>>& treasures);
    void addDevil(const Hex& hex, std::vector<std::shared_ptr<Devil>>& devils);
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const;
    bool HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const;
};

#endif