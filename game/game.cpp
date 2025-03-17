#include "game.hpp"
#include <random>
#include <map>
#include <filesystem>
#include <algorithm>

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
      playerTurn(0),  // Start with player 0
      entitySelected(false),
      selectedEntityIndex(-1)
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
            players.emplace_back(color);
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
        SDL_Color playerColor = player.getColor();
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
            player.addEntity(villager);
            auto  pikeman = std::make_shared<Pikeman>(randomHexPikeman);
            player.addEntity(pikeman);
        }
    }
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
        for (const auto& entity : player.getEntities()) {
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

void Game::handleEvent(SDL_Event& event) {
    // if 'E' is pressed, change player
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_e) {
            // Change player
            playerTurn = (playerTurn + 1) % players.size();

            // BANDIT ACTIONS HERE
            if (playerTurn == 0) {
                turn++;
                if(turn > 0) {
                    manageBandits();
                }
            }
            // END OF BANDIT ACTIONS

            Player& currentPlayer = players[playerTurn];
            if(turn > 0) {
                // land income
                currentPlayer.addCoins(grid.getNbCasesColor(currentPlayer.getColor()));
                // coins stolen by bandits
                currentPlayer.removeCoins(nbBanditsOnColor(currentPlayer.getColor()));

                // Pay upkeep for each entity
                for(auto& entity : currentPlayer.getEntities()) {
                    entity->setMoved(false);
                    int currentUpkeep = entity->getUpkeep();
                    if(currentPlayer.getCoins() >= currentUpkeep) {
                        currentPlayer.removeCoins(currentUpkeep);
                    } else {
                        // If cannot pay upkeep, replace by bandit
                        Hex entityHex = entity->getHex();
                        currentPlayer.removeEntity(entity);
                        addBandit(entityHex);
                    }
                }
            }
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY);

        if (playerTurn >= 0 && playerTurn < players.size()) {
            if (!entitySelected) {
                Player& currentPlayer = players[playerTurn];
                const auto& playerEntities = currentPlayer.getEntities();
                
                for (size_t i = 0; i < playerEntities.size(); ++i) {
                    if (!playerEntities[i]->hasMoved() && playerEntities[i]->getHex() == clickedHex) {
                        selectedEntityIndex = i;
                        entitySelected = true;
                        break;
                    }
                }
            } else {
                bool moveSuccessful = false;
                if (grid.hexExists(clickedHex)) {
                    Player& currentPlayer = players[playerTurn];
                    auto entity = currentPlayer.getEntities()[selectedEntityIndex];
                    
                    if ((entity) && 
                        !isSurroundedByOtherPlayerEntities(clickedHex, currentPlayer, entity->getProtectionLevel()) &&
                        !isSurroundedBySamePlayerEntities(clickedHex, currentPlayer)) {
                        moveSuccessful = entity->move(grid, clickedHex, currentPlayer.getColor());
                        
                        // We flag the entity as moved if the move was successful
                        if (moveSuccessful) {
                            entity->setMoved(true);
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

void Game::render(SDL_Renderer* renderer) const {
    // Draw the grid
    grid.draw(renderer);

    // Render all bandits
    for (const auto& bandit : bandits) {
        render_entity(renderer, *bandit, textures[iconsMap.at("bandit")]);
    }

    Player currentPlayer = players[playerTurn];

    // Render of the entity on the cursor if selected
    if(entitySelected) {
        const auto& playerEntities = currentPlayer.getEntities();
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
        for (const auto& entity : player.getEntities()) {
            // Skip rendering the selected entity if it belongs to the current player
            if(entitySelected && entity == currentPlayer.getEntities()[selectedEntityIndex]) {
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
        SDL_Color currentColor = players[playerTurn].getColor();
        
        SDL_Rect colorRect = {10, 10, 50, 30};
        
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        
        SDL_RenderFillRect(renderer, &colorRect);
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &colorRect);
    }
    std::string coinsnumber = std::to_string(currentPlayer.getCoins());
    // coinTexture = textures[iconsMap.at("coin")];
    SDL_Rect coinRect = {10, 50, 50, 30};
    SDL_RenderCopy(renderer, textures[iconsMap.at("coin")], NULL, &coinRect);
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Rect textRect = {70, 50, 50, 30};
    SDL_RenderCopy(renderer, SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(font, coinsnumber.c_str(), textColor)), NULL, &textRect);
    TTF_CloseFont(font);
}

bool Game :: isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel) {
    // Define the directions to the six neighbors
    const std::vector<Hex> directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    };
    
    for (auto& player : players) {
        if (player.getColor() == currentPlayer.getColor()) {
            continue;
        }
        
        for (const auto& direction : directions) {
            Hex neighbor = hex.add(direction);
            if (grid.hexExists(neighbor)) {
                for (const auto& entity : player.getEntities()) {
                    if (entity->getHex() == neighbor && 
                        entity->getProtectionLevel() >= currentLevel && 
                        grid.getHexColors().at(hex) == player.getColor()) {
                        std::cout << "Hex protected by another player's entity" << std::endl;
                        return true;
                    }
                }
            }
        }

        for (const auto& entity : player.getEntities()) {
            if (entity->getHex() == hex &&  
                grid.getHexColors().at(hex) == player.getColor()) {
                if (entity->getProtectionLevel() >= currentLevel) {
                    std::cout << "Hex protected by another player's entity" << std::endl;
                    return true;
                } else {
                    player.removeEntity(entity);
                    std::cout << "Entity remove" << std::endl;
                    return false;
                }
            }
        }
    }
    return false;
}

// Check if a hex is surrounded other entities of the same player
bool Game::isSurroundedBySamePlayerEntities(const Hex& hex, const Player& currentPlayer) const {
    for (const auto& entity : currentPlayer.getEntities()) {
        if (entity->getHex() == hex) {
            return true;
        }
    }
    return false;
}