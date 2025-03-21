#include "game.hpp"
#include <random>
#include <map>
#include <filesystem>
#include <algorithm>
#include <queue>
#include <set>

std::map<std::string, int> iconsMap = {
    {"bandit", 0},        {"bandit-camp", 1},
    {"castle", 2},        {"coin", 3},
    {"coins", 4},         {"deficit", 5},
    {"emoji-happy", 6},   {"face", 7},
    {"gold-trophy", 8},   {"grave", 9},
    {"hero", 10},         {"knight", 11},
    {"pikeman", 12},      {"silver-trophy", 13},
    {"surplus", 14},      {"town", 15},
    {"treasury", 16},     {"upkeep", 17},
    {"villager", 18}
};

// Custom comparison function for SDL_Color
struct SDL_Color_Compare {
    bool operator()(const SDL_Color& a, const SDL_Color& b) const {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return a.a < b.a;
    }
};

Game::Game(double hexSize, const std::vector<std::string>& asciiMap,
        int windowWidth, int windowHeight, SDL_Renderer* renderer)
    : grid(hexSize),
    playerTurn(0),
    entitySelected(false),
    selectedEntityIndex(-1),
    draggedButton(nullptr)
{
    std::cout << "Game constructor started" << std::endl;
    turn = 0;

    // Load all textures from the icons directory
    std::cout << "Loading textures..." << std::endl;
    std::string iconsPath = "icons/";
    for (const auto& filename : iconsMap) {
        std::string path = iconsPath + filename.first + ".png";
        std::cout << "Loading texture: " << path << std::endl;
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (!texture) {
            std::cerr << "Error loading texture: " << IMG_GetError() << std::endl;
        }
        textures.push_back(texture);
    }
    std::cout << "Textures loaded: " << textures.size() << std::endl;

    std::cout << "Generating grid..." << std::endl;
    grid.generateFromASCII(asciiMap, windowWidth, windowHeight);
    std::cout << "Grid generated" << std::endl;

    // Count the number of unique colors in the grid and create players
    nbplayers = 0;
    std::vector<SDL_Color> uniqueColors;
    for (const auto& pair : grid.getHexColors()) {
        const SDL_Color& color = pair.second;
        bool found = false;
        for (const auto& uniqueColor : uniqueColors) {
            if (color == uniqueColor) {
                found = true;
                break;
            }
        }
        if (!found && !(color == SDL_Color{255, 255, 255, 255})) {
            nbplayers++;
            players.emplace_back(std::make_shared<Player>(color));
            uniqueColors.push_back(color);
        }
    }
    std::cout << "Number of players: " << nbplayers << std::endl;

    // Initialize a bandit at a random white hex
    Hex initialBanditHex = grid.getHexes()[0];
    bool banditHexFound = false;
    for (const auto& pair : grid.getHexColors()) {
        if (pair.second == SDL_Color{255, 255, 255, 255}) {
            initialBanditHex = pair.first;
            banditHexFound = true;
            break;
        }
    }
    if (!banditHexFound && !grid.getHexes().empty()) {
        initialBanditHex = grid.getHexes()[0];
    }
    bandits.push_back(std::make_shared<Bandit>(initialBanditHex));

    // Create a map to store hexes by color
    std::map<SDL_Color, std::vector<Hex>, SDL_Color_Compare> hexesByColor;
    for (const auto& pair : grid.getHexColors()) {
        SDL_Color color = pair.second;
        hexesByColor[color].push_back(pair.first);
    }

    // Initialize villagers for each player
    std::random_device rd;
    std::mt19937 gen(rd());

    for (auto& player : players) {
        SDL_Color playerColor = player->getColor();
        auto it = hexesByColor.find(playerColor);

        if (it != hexesByColor.end()) {
            std::vector<Hex>& playerHexes = it->second;
            if (playerHexes.empty()) {
            std::cerr << "No hexes found for player color: " << playerColor.r << ", " << playerColor.g << ", " << playerColor.b << std::endl;
            continue;
            }

            // Choose a random hex for the villager
            std::uniform_int_distribution<> distrib(0, playerHexes.size() - 1);
            int randomIndexVillager = distrib(gen);
            Hex randomHexVillager = playerHexes[randomIndexVillager];

            // Choose a random hex for the pikeman (different)
            int randomIndexPikeman = distrib(gen);
            while(randomIndexPikeman == randomIndexVillager) {
                randomIndexPikeman = distrib(gen);
            }
            Hex randomHexPikeman = playerHexes[randomIndexPikeman];

            // Create a villager and add it to the player
            auto villager = std::make_shared<Villager>(randomHexVillager);
            player->addEntity(villager);
            auto pikeman = std::make_shared<Town>(randomHexPikeman);
            player->addEntity(pikeman);
        }
    }
    int nbButtons = 5;
    int buttonSize = 50;
    int buttonSpacing = 10;
    int totalWidth = nbButtons * buttonSize + 3 * buttonSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int buttonY = windowHeight - buttonSize - 20;

    unitButtons.emplace_back(startX, buttonY, buttonSize, buttonSize, "villager", 10);
    unitButtons.emplace_back(startX + buttonSize + buttonSpacing, buttonY, buttonSize, buttonSize, "pikeman", 20);
    unitButtons.emplace_back(startX + 2 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "knight", 40);
    unitButtons.emplace_back(startX + 3 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "hero", 80);
    unitButtons.emplace_back(startX + 4 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "castle", 20);
}

Game::~Game() {
    for (SDL_Texture* texture : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
}

void Game::addBandit(Hex hex) {
    bandits.push_back(std::make_shared<Bandit>(hex));
}

bool Game::entityOnHex(const Hex& hex) {
    for(const auto& bandit : bandits) {
        if (bandit->getHex() == hex) {
            return true;
        }
    }
    for (const auto& player : players) {
        for (const auto& entity : player->getEntities()) {
            if (entity->getHex() == hex) {
                return true;
            }
        }
    }
    return false;
}

int Game::nbBanditsOnColor(const SDL_Color& color) {
    int count = 0;
    for (const auto& bandit : bandits) {
        if (grid.getHexColors().at(bandit->getHex()) == color) {
            count++;
        }
    }
    return count;
}

void Game::manageBandits(){
    // Directions possibles sur une grille hexagonale (pointy-top orientation)
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };

    for (const auto& bandit : bandits) {
        bool moved = false;
        int maxAttempts = 10; // Limite le nombre de tentatives
        int attempts = 0;

        // Essayer de trouver une position valide
        while (!moved && attempts < maxAttempts) {
            // Choisir une direction aléatoire
            Hex direction = directions[std::rand() % directions.size()];

            // Calculer la nouvelle position
            Hex newHex = bandit->getHex().add(direction);

            // Vérifier si la nouvelle position est valide
            if (grid.hexExists(newHex) && !entityOnHex(newHex)) {
                bandit->move(grid, newHex);
                moved = true;
            } else {
                attempts++;
            }
        }
    }
}

void Game::upgradeEntity(const Hex& hex) {
    for (auto& player : players) {
        for (auto& entity : player->getEntities()) {
            if (entity->getHex() == hex) {
                bool hasmoved = entity->hasMoved();
                if (entity->getName() == "villager") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Pikeman>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                } else if (entity->getName() == "pikeman") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Knight>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                } else if (entity->getName() == "knight") {
                    player->removeEntity(entity);
                    player->addEntity(std::make_shared<Hero>(hex));
                    player->getEntities().back()->setMoved(hasmoved);
                    return;
                }
            }
        }
    }
}

void Game::handleEvent(SDL_Event& event) {
    // if 'E' is pressed, change player
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_e) {
            // if entities on hex not existing on the grid refund the cost of the entity
            if(entitySelected) {
                auto& entity = players[playerTurn]->getEntities()[selectedEntityIndex];
                Hex entityHex = entity->getHex();
                if(!(grid.hexExists(entityHex))) {
                    players[playerTurn]->removeEntity(entity);
                    // refund the cost of the entity
                    for(auto& button : unitButtons) {
                        if(button.getIconName() == entity->getName()) {
                            players[playerTurn]->addCoins(button.getCost());
                        }
                    }
                }
            }
            selectedEntityIndex = -1;
            entitySelected = false;

            for(auto& player : players) {
                checkIfHexConnectedToTown(*player);
            }
            
            // Change player
            playerTurn = (playerTurn + 1) % players.size();
            auto& currentPlayer = players[playerTurn];
            SDL_Color currentColor = currentPlayer->getColor();

            // Remove dead players
            for (auto it = players.rbegin(); it != players.rend(); ++it) {
                auto& player = *it;
                if (!player->isAlive()) {
                    removePlayer(player);
                    if(players.size() <= playerTurn){
                        playerTurn = 0;
                    }
                    while (!(currentColor == players[playerTurn]->getColor())) {
                        playerTurn = (playerTurn + 1) % players.size();
                    }
                    currentPlayer = players[playerTurn];
                }
            }

            // BANDIT ACTIONS HERE
            if (playerTurn == 0) {
                turn++;
                if(turn > 0) {
                    manageBandits();
                }
            }
            // END OF BANDIT ACTIONS

            if(turn > 0) {
                // land income
                currentPlayer->addCoins(grid.getNbCasesColor(currentPlayer->getColor()));
                // coins stolen by bandits
                currentPlayer->removeCoins(nbBanditsOnColor(currentPlayer->getColor()));

                // Pay upkeep for each entity in reverse order (because we remove entities)
                for(auto it = currentPlayer->getEntities().rbegin(); it != currentPlayer->getEntities().rend(); ++it) {
                    auto& entity = *it;
                    bool building = dynamic_cast<Building*>(entity.get());

                    if(!building) {
                        entity->setMoved(false);
                    }
                    int currentUpkeep = entity->getUpkeep();

                    if(currentPlayer->getCoins() >= currentUpkeep) {
                        currentPlayer->removeCoins(currentUpkeep);
                    } else {
                        if(building) {
                            currentPlayer->removeEntity(entity);
                            // add bandit camp
                        } else {
                            // If cannot pay upkeep, replace by bandit
                            Hex entityHex = entity->getHex();
                            currentPlayer->removeEntity(entity);
                            addBandit(entityHex);
                        }
                    }
                }
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY);

        // Check if a button was clicked
        for (auto& button : unitButtons) {
            if (button.containsPoint(mouseX, mouseY) && !entitySelected) {
                if (button.getCost() <= players[playerTurn]->getCoins()) {
                    if (button.getIconName() == "villager") {
                        players[playerTurn]->addEntity(std::make_shared<Villager>(clickedHex));
                    } else if (button.getIconName() == "pikeman") {
                        players[playerTurn]->addEntity(std::make_shared<Pikeman>(clickedHex
                        ));
                    } else if (button.getIconName() == "knight") {
                        players[playerTurn]->addEntity(std::make_shared<Knight>(clickedHex));
                    } else if (button.getIconName() == "hero") {
                        players[playerTurn]->addEntity(std::make_shared<Hero>(clickedHex));
                    } else if(button.getIconName() == "castle") {
                        players[playerTurn]->addEntity(std::make_shared<Castle>(clickedHex));
                        players[playerTurn]->getEntities().back()->setMoved(false);
                    }
                    players[playerTurn]->removeCoins(button.getCost());
                }
            }
        }


        if (playerTurn >= 0 && playerTurn < players.size()) {
            if (!entitySelected) {
                auto& currentPlayer = players[playerTurn];
                const auto& playerEntities = currentPlayer->getEntities();

                for (size_t i = 0; i < playerEntities.size(); ++i) {
                    if (!playerEntities[i]->hasMoved() && playerEntities[i]->getHex() == clickedHex) {
                        if(dynamic_cast<Building*>(playerEntities[i].get()) && grid.hexExists(clickedHex)) {
                            continue;
                        }
                        selectedEntityIndex = i;
                        entitySelected = true;
                        break;
                    }
                }
            } else {
                bool moveSuccessful = false;
                if (grid.hexExists(clickedHex)) {
                    auto& currentPlayer = players[playerTurn];
                    auto entity = currentPlayer->getEntities()[selectedEntityIndex];

                    if ((entity) &&
                        !isSurroundedByOtherPlayerEntities(clickedHex, *currentPlayer, entity->getProtectionLevel()) &&
                        hasSamePlayerEntities(clickedHex, *currentPlayer) == "") {
                        if(entity->getName() == "castle" && grid.getHexColors().at(clickedHex) == currentPlayer->getColor()) {
                            entity->setHex(clickedHex);
                            entity->setMoved(true);
                        }
                        moveSuccessful = entity->move(grid, clickedHex, currentPlayer->getColor());

                        // We flag the entity as moved if the move was successful
                        if (moveSuccessful) {
                            entity->setMoved(true);

                            // remove potential entity on the hex we are moving to
                            for (auto& player : players) {
                                if (player->getColor() == currentPlayer->getColor()) {
                                    continue;
                                }
                                for (auto& entity : player->getEntities()) {
                                    if (entity->getHex() == clickedHex) {
                                        if (entity->getName() == "town") {
                                            player->setAlive(false);
                                        }
                                        player->removeEntity(entity);
                                        printf("Entity removed\n");
                                        break;
                                    }
                                }
                            }
                        }
                    } else if (entity && !(entity->getHex() == clickedHex) && entity->getName() == hasSamePlayerEntities(clickedHex, *currentPlayer) && entity->getName() != "hero") {
                        currentPlayer->removeEntity(entity);
                        upgradeEntity(clickedHex);
                    }
                } else {
                    // handle when the hex in question is on a button (then doesnt exist)
                    auto& entity = players[playerTurn]->getEntities()[selectedEntityIndex];
                    Hex entityHex = entity->getHex();
                    if(!(grid.hexExists(entityHex))) {
                        players[playerTurn]->removeEntity(entity);
                        // refund the cost of the entity
                        for(auto& button : unitButtons) {
                            if(button.getIconName() == entity->getName()) {
                                players[playerTurn]->addCoins(button.getCost());
                            }
                        }
                    }
                }
                entitySelected = false;
                selectedEntityIndex = -1;
            }
        }
    } else if (event.type == SDL_MOUSEMOTION) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        grid.handleMouseMotion(mouseX, mouseY);
    }
}

void Game::update() {
}

SDL_Rect Game::entityToRect(const Entity& entity) const {
    int x = static_cast<int>(grid.hexToPixel(entity.getHex()).x - grid.getHexSize() / 2);
    int y = static_cast<int>(grid.hexToPixel(entity.getHex()).y - grid.getHexSize() / 2);
    int w_h = static_cast<int>(grid.getHexSize());
    return {x, y, w_h, w_h};
}

void Game::render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture) const {
    SDL_Rect entityRect = entityToRect(entity);
    SDL_RenderCopy(renderer, texture, NULL, &entityRect);
}

void Game::renderButton(SDL_Renderer* renderer, const Button& button) const {
    SDL_Rect buttonRect = button.getRect();

    // Draw button background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    // Draw button icon
    SDL_Texture* iconTexture = textures[iconsMap.at(button.getIconName())];
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);

    // Draw cost text with better visibility
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 16); // Larger font size
    if (font) {
        // Create a small background for the text
        SDL_Rect textBgRect = {
            buttonRect.x,
            buttonRect.y + buttonRect.h - 20,
            buttonRect.w,
            20
        };

        // Draw a semi-transparent black background for the text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &textBgRect);

        // Use bright text color for better contrast
        SDL_Color textColor = {255, 255, 0, 255}; // Bright yellow for visibility

        std::string costText = std::to_string(button.getCost());
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, costText.c_str(), textColor);

        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            if (textTexture) {
                // Center the text in the background
                int textWidth = textSurface->w;
                int textHeight = textSurface->h;

                SDL_Rect costRect = {
                    buttonRect.x + (buttonRect.w - textWidth) / 2,
                    buttonRect.y + buttonRect.h - textHeight - 2,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, textTexture, NULL, &costRect);
                SDL_DestroyTexture(textTexture);
            }

            SDL_FreeSurface(textSurface);
        }

        TTF_CloseFont(font);
    }
}

void Game::render(SDL_Renderer* renderer) const {
    // Draw the grid
    grid.draw(renderer);

    // Render all bandits
    for (const auto& bandit : bandits) {
        render_entity(renderer, *bandit, textures[iconsMap.at("bandit")]);
    }

    auto& currentPlayer = players[playerTurn];

    // Render of the entity on the cursor if selected
    if(entitySelected) {
        const auto& playerEntities = currentPlayer->getEntities();
        const Entity& selectedEntity = *playerEntities[selectedEntityIndex];
        SDL_Rect entityRect = entityToRect(selectedEntity);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        entityRect.x = mouseX - entityRect.w / 2;
        entityRect.y = mouseY - entityRect.h / 2;
        SDL_RenderCopy(renderer, textures[iconsMap.at(selectedEntity.getName())], NULL, &entityRect);
    }

    // Render all players' entities
    for (size_t i = 0; i < players.size(); i++) {
        const auto& player = players[i];
        for (const auto& entity : player->getEntities()) {
            // Skip rendering the selected entity if it belongs to the current player
            if(entitySelected && entity == currentPlayer->getEntities()[selectedEntityIndex]) {
                continue;
            }
            // Determine the texture based on entity type
            std::string textureKey = entity->getName();

            // Render the entity
            render_entity(renderer, *entity, textures[iconsMap.at(textureKey)]);
        }
    }

    // Display current player's color at the top of the screen
    if (playerTurn >= 0 && playerTurn < players.size()) {
        SDL_Color currentColor = players[playerTurn]->getColor();

        SDL_Rect colorRect = {10, 10, 50, 30};

        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);

        SDL_RenderFillRect(renderer, &colorRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &colorRect);
    }
    // coinTexture = textures[iconsMap.at("coin")];
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    RenderData renderData(renderer, font, textures, iconsMap);

    // Remplacez le code existant par un appel à renderImageWithText
    std::string coinsnumber = std::to_string(currentPlayer->getCoins());
    SDL_Rect coinRect = {10, 60, 50, 30};
    if (currentPlayer->getCoins() < 50) {
        renderData.renderImageWithText(coinRect, "coin", coinsnumber);
    } else {
        renderData.renderImageWithText(coinRect, "coins", coinsnumber);
    }
    int totalupkeep = grid.getNbCasesColor(currentPlayer->getColor());
    for (const auto& entity : currentPlayer->getEntities()) {
        totalupkeep -= entity->getUpkeep();
    }
    std::string stringupkeep = std::to_string(totalupkeep);
    SDL_Rect upkeepRect = {10, 100, 50, 60};
    if (totalupkeep < 0) {
        renderData.renderImageWithText(upkeepRect, "deficit", stringupkeep);
    } else {
        renderData.renderImageWithText(upkeepRect, "surplus", stringupkeep);
    }
    TTF_CloseFont(font);

    // Render all buttons
    for (const auto& button : unitButtons) {
        renderButton(renderer, button);
    }
}

bool Game::isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel) {
    // Define the directions to the six neighbors
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };

    for (auto& player : players) {
        if (player->getColor() == currentPlayer.getColor()) {
            continue;
        }

        for (const auto& direction : directions) {
            Hex neighbor = hex.add(direction);
            if (grid.hexExists(neighbor)) {
                for (const auto& entity : player->getEntities()) {
                    if (entity->getHex() == neighbor &&
                        entity->getProtectionLevel() >= currentLevel &&
                        grid.getHexColors().at(hex) == player->getColor()) {
                        std::cout << "Hex protected by another player's entity" << std::endl;
                        return true;
                    }
                }
            }
        }

        for (const auto& entity : player->getEntities()) {
            if (entity->getHex() == hex &&
                grid.getHexColors().at(hex) == player->getColor()) {
                if (entity->getProtectionLevel() >= currentLevel) {
                    std::cout << "Too weak to beat this enemy" << std::endl;
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
}

// Check if a hex is surrounded other entities of the same player
std::string Game::hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const {
    for (const auto& entity : currentPlayer.getEntities()) {
        if (entity->getHex() == hex) {
            return entity->getName();
        }
    }
    return "";
}

// Remove a player from the game
void Game::removePlayer(std::shared_ptr<Player> player) {
    auto entities = player->getEntities();
    for(auto& entity : entities) {
        if (entity) {
            if (dynamic_cast<Building*>(entity.get())) {
                // addBanditCamp(entity->getHex());
                player->removeEntity(entity);
            } else {
                addBandit(entity->getHex());
                player->removeEntity(entity);
            }
        }
    }
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        players.erase(it);
    } else {
        std::cerr << "Error: Player not found in the players vector." << std::endl;
    }
}

void Game::checkIfHexConnectedToTown(Player& player) {
    // Get the player's color
    SDL_Color playerColor = player.getColor();
    
    // Find all hexes with this player's color
    std::vector<Hex> playerHexes;
    for (const auto& pair : grid.getHexColors()) {
        if (pair.second == playerColor) {
            playerHexes.push_back(pair.first);
        }
    }
    
    // Find all town hexes (shouldnt be more than 1 but who knows what we gonna do in the future)
    std::vector<Hex> townHexes;
    for (const auto& entity : player.getEntities()) {
        if (entity->getName() == "town") {
            townHexes.push_back(entity->getHex());
        }
    }
    
    // If no towns, all hexes are disconnected
    if (townHexes.empty()) {
        for (const Hex& hex : playerHexes) {
            disconnectHex(player, hex);
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
    
    // Define directions for adjacent hexes
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };
    
    // BFS traversal
    while (!queue.empty()) {
        Hex current = queue.front();
        queue.pop();
        
        // Check all adjacent hexes
        for (const Hex& dir : directions) {
            Hex neighbor = current.add(dir);
            
            // If neighbor is valid, has player's color, and hasn't been visited yet
            if (grid.hexExists(neighbor) && 
                grid.getHexColors().at(neighbor) == playerColor &&
                connectedHexes.find(neighbor) == connectedHexes.end()) {
                
                connectedHexes.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    // Handle disconnected hexes
    for (const Hex& hex : playerHexes) {
        if (connectedHexes.find(hex) == connectedHexes.end()) {
            disconnectHex(player, hex);
        }
    }
}

void Game::disconnectHex(Player& player, const Hex& hex) {
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
                //addBanditCamp(hex);
            } else {
                addBandit(hex);
            }
            player.removeEntity(entity);
            break;
        }
    }
}
