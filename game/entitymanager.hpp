#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <limits>

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"
#include "../players/player.hpp"

class EntityManager {
public:
    void generateEntities(const std::vector<std::string>& entityMap, const std::vector<std::string>& asciiMap, HexagonalGrid& grid, std::vector<std::shared_ptr<Player>>& players, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, std::vector<std::shared_ptr<Treasure>>& treasures);
    void upgradeEntity(const Hex& hex, std::vector<std::shared_ptr<Player>>& players);
    bool entityOnHex(const Hex& hex, const std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players) const;
    void manageBandits(HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, std::vector<std::shared_ptr<Treasure>>& treasures, std::vector<std::shared_ptr<Devil>>& devils, std::vector<std::shared_ptr<Player>>& players);
    void addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits);
    void addBanditCamp(const Hex& hex, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
    void addTreasure(const Hex& hex, int value, std::vector<std::shared_ptr<Treasure>>& treasures);
    void addDevil(const Hex& hex, std::vector<std::shared_ptr<Devil>>& devils);
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const;
    bool HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Devil>>& devils) const;
private:
    void addEntityToPlayer(char entityType, const Hex& hex, std::shared_ptr<Player>& player);
    void moveBanditToNewPosition(HexagonalGrid& grid, std::shared_ptr<Bandit>& bandit, const std::vector<Hex>& directions, const std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players);
    void stealCoinFromPlayer(HexagonalGrid& grid, const std::shared_ptr<Bandit>& bandit, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Player>>& players);
    void spawnBanditFromCamp(HexagonalGrid& grid, std::shared_ptr<BanditCamp>& banditCamp, const std::vector<Hex>& directions, std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players);
};

#endif