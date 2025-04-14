#include "playermanager.hpp"

// Check if a hex is surrounded other entities of the same player
std::string PlayerManager::hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const {
    for (const auto& entity : currentPlayer.getEntities()) {
        if (entity->getHex() == hex) {
            return entity->getName();
        }
    }
    return "";
}

void PlayerManager::removePlayer(std::shared_ptr<Player> player, std::vector<std::shared_ptr<Player>>& players, int& nbplayers, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps) {
    auto entities = player->getEntities();
    // vector "toRemove" to store the entities to remove to avoid modifying the vector while iterating over it
    std::vector<std::shared_ptr<Entity>> toRemove;
    for(auto& entity : entities) {
        if (entity) {
            if (dynamic_cast<Building*>(entity.get())) {
                entityManager.addBanditCamp(entity->getHex(), banditCamps);
            } else {
                entityManager.addBandit(entity->getHex(), bandits);
            }
            toRemove.push_back(entity);
        }
    }
    for(auto& entity : toRemove) {
        player->removeEntity(entity);
    }

    players.erase(std::remove(players.begin(), players.end(), player), players.end());
    nbplayers--;
}

void PlayerManager::checkIfHexConnectedToTown(Player& player, HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps) {
    // Get the player's color
    SDL_Color playerColor = player.getColor();
    SDL_Color originalColor = playerColor; // Store original player color
    
    // Find all hexes with this player's color or darker versions of it
    std::vector<Hex> playerHexes;
    std::map<Hex, SDL_Color> currentHexColors; // Store current colors for potential restoration
    
    for (const auto& pair : grid.getHexColors()) {
        const SDL_Color& hexColor = pair.second;
        // Check if this is the player's color or a darker version of it (not sure about how clean the > * 0.69 is, but it works)
        if (hexColor == playerColor || 
            (hexColor.r <= playerColor.r && hexColor.g <= playerColor.g && 
             hexColor.b <= playerColor.b && 
             hexColor.r >= playerColor.r * 0.69 && hexColor.g >= playerColor.g * 0.69 && 
             hexColor.b >= playerColor.b * 0.69)) {
            
            playerHexes.push_back(pair.first);
            currentHexColors[pair.first] = hexColor;
        }
    }
    
    // Find all town hexes
    std::vector<Hex> townHexes;
    for (const auto& entity : player.getEntities()) {
        if (entity->getName() == "town") {
            townHexes.push_back(entity->getHex());
        }
    }
    
    // If no towns, all hexes are disconnected
    if (townHexes.empty()) {
        for (const Hex& hex : playerHexes) {
            disconnectHex(player, hex, grid, bandits, banditCamps);
        }
        return;
    }
    
    // Use BFS to find all hexes connected to any town
    std::set<Hex> connectedHexes;
    std::queue<Hex> queue;
    
    // Start BFS from each town
    for (const Hex& townHex : townHexes) {
        queue.push(townHex);
        connectedHexes.insert(townHex);
    }
    
    // BFS traversal
    while (!queue.empty()) {
        Hex current = queue.front();
        queue.pop();
        
        // If current hex has a darker color, restore it to the player's original color
        if (!(grid.getHexColors().at(current) == playerColor)) {
            grid.setHexColor(current, originalColor);
        }
        
        // Check all adjacent hexes
        for (const Hex& dir : directions) {
            Hex neighbor = current.add(dir);
            
            // If neighbor exists and has any version of player's color (original or darker)
            if (grid.hexExists(neighbor) && 
                currentHexColors.find(neighbor) != currentHexColors.end() &&
                connectedHexes.find(neighbor) == connectedHexes.end()) {
                
                connectedHexes.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    // Handle disconnected hexes
    for (const Hex& hex : playerHexes) {
        if (connectedHexes.find(hex) == connectedHexes.end()) {
            disconnectHex(player, hex, grid, bandits, banditCamps);
        }
    }
}


void PlayerManager::disconnectHex(Player& player, const Hex& hex, HexagonalGrid& grid, std::vector<std::shared_ptr<Bandit>>& bandits, std::vector<std::shared_ptr<BanditCamp>>& banditCamps) {
    // Make the color darker
    SDL_Color newHexColor = player.getColor();
    newHexColor.r = newHexColor.r * 0.7;
    newHexColor.g = newHexColor.g * 0.7;
    newHexColor.b = newHexColor.b * 0.7;
    grid.setHexColor(hex, newHexColor);
    
    // Remove potential entity on this hex and replace with bandits
    std::vector<std::shared_ptr<Entity>> entitiesToRemove;
    
    for (auto& entity : player.getEntities()) {
        if (entity->getHex() == hex) {
            if (dynamic_cast<Building*>(entity.get())) {
                entityManager.addBanditCamp(hex, banditCamps);
            } else {
                entityManager.addBandit(hex, bandits);
            }
            player.removeEntity(entity);
            break;
        }
    }
}