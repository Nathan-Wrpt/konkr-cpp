#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <random>

#include "../game/gameentities.hpp"

class EntityManager {
public:
    void generateEntities(const std::vector<std::string>& entityMap, const std::vector<std::string>& asciiMap, HexagonalGrid& grid, GameEntities& gameEntities);
    void upgradeEntity(const Hex& hex, std::vector<std::shared_ptr<Player>>& players);
    bool entityOnHex(const Hex& hex, const GameEntities& gameEntities) const;
    void manageBandits(HexagonalGrid& grid, GameEntities& gameEntities);
    void addBandit(const Hex& hex, std::vector<std::shared_ptr<Bandit>>& bandits);
    void addBanditCamp(const Hex& hex, std::vector<std::shared_ptr<BanditCamp>>& banditCamps);
    void addTreasure(const Hex& hex, int value, std::vector<std::shared_ptr<Treasure>>& treasures);
    void addDevil(const Hex& hex, std::vector<std::shared_ptr<Devil>>& devils);
    void addForest(const Hex& hex, std::vector<std::shared_ptr<Forest>>& forests);
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel, const HexagonalGrid& grid, const GameEntities& gameEntities) const;
    bool HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex, const HexagonalGrid& grid, size_t playerTurn, const GameEntities& gameEntities) const;
    Hex randomfreeHex(const HexagonalGrid& grid, const GameEntities& gameEntities) const;
private:
    void addEntityToPlayer(char entityType, const Hex& hex, std::shared_ptr<Player>& player);
    void moveBanditToNewPosition(HexagonalGrid& grid, std::shared_ptr<Bandit>& bandit, const GameEntities& gameEntities);
    void stealCoinFromPlayer(HexagonalGrid& grid, const std::shared_ptr<Bandit>& bandit, std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Player>>& players);
    void spawnBanditFromCamp(HexagonalGrid& grid, std::shared_ptr<BanditCamp>& banditCamp, std::vector<std::shared_ptr<Bandit>>& bandits, const GameEntities& gameEntities);
};

#endif