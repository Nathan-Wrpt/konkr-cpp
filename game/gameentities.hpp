#ifndef GAMEENTITIES_HPP
#define GAMEENTITIES_HPP

#include "../players/player.hpp"

struct GameEntities {
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Bandit>> bandits;
    std::vector<std::shared_ptr<BanditCamp>> banditCamps;
    std::vector<std::shared_ptr<Treasure>> treasures;
    std::vector<std::shared_ptr<Devil>> devils;
    std::vector<std::shared_ptr<Forest>> forests;
};

#endif