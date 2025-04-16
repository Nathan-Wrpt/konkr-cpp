#include "game.hpp"
#include <random>
#include <map>
#include <filesystem>
#include <algorithm>

// Custom comparison function for SDL_Color
struct SDL_Color_Compare {
    bool operator()(const SDL_Color& a, const SDL_Color& b) const {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return a.a < b.a;
    }
};

Game::Game(double hexSize, const std::vector<std::string>& asciiMap, std::vector<std::string>& entityMap,
        int windowWidth, int windowHeight, SDL_Renderer* renderer, int cameraSpeed)
    : grid(hexSize),
    playerTurn(0),
    entitySelected(false),
    selectedEntityIndex(-1),
    turnButton(0, 0, 0, 0, "", 0),
    undoButton(0, 0, 0, 0, "", 0),
    quitButton(0, 0, 0, 0, "", 0),
    replayButton(0, 0, 0, 0, "", 0),
    draggedButton(nullptr),
    cameraSpeed(cameraSpeed),
    endGame(false),
    hoveredButton(0, 0, 0, 0, "", 0)
{
    std::cout << "Game constructor started" << std::endl;
    turn = 0;

    cameraX = 0;
    cameraY = 0;
    // Load all textures from the icons directory
    std::cout << "Loading textures..." << std::endl;
    std::string iconsPath = "icons/";
    for (const auto& filename : iconNames) {
        std::string path = iconsPath + filename + ".png";
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
        if (!found && !(color == defaultColor)) {
            nbplayers++;
            gameEntities.players.emplace_back(std::make_shared<Player>(color));
            uniqueColors.push_back(color);
        }
    }
    std::cout << "Number of players: " << nbplayers << std::endl;

    // Create a map to store hexes by color
    std::map<SDL_Color, std::vector<Hex>, SDL_Color_Compare> hexesByColor;
    for (const auto& pair : grid.getHexColors()) {
        SDL_Color color = pair.second;
        hexesByColor[color].push_back(pair.first);
    }

    // Generate entities based on the entityMap
    std::cout << "Generating entities..." << std::endl;
    entityManager.generateEntities(entityMap, asciiMap, grid, gameEntities);

    int nbButtons = 5;
    int buttonSize = 50;
    int buttonSpacing = 10;
    int totalWidth = nbButtons * buttonSize + 3 * buttonSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int buttonY = windowHeight - buttonSize - 20 - buttonSpacing;

    unitButtons.emplace_back(startX, buttonY, buttonSize, buttonSize, "villager", 10);
    unitButtons.emplace_back(startX + buttonSize + buttonSpacing, buttonY, buttonSize, buttonSize, "pikeman", 20);
    unitButtons.emplace_back(startX + 2 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "knight", 40);
    unitButtons.emplace_back(startX + 3 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "hero", 80);
    unitButtons.emplace_back(startX + 4 * (buttonSize + buttonSpacing), buttonY, buttonSize, buttonSize, "castle", 20);

    // turn button on the bottom right corner
    int turnButtonWidth = buttonSize * 3;
    turnButton = Button(windowWidth - turnButtonWidth- 20, windowHeight - buttonSize - 20, turnButtonWidth, buttonSize, "next", 0);
    undoButton = Button(windowWidth - 2 * turnButtonWidth - 2 * 20, windowHeight - buttonSize - 20, turnButtonWidth, buttonSize, "undo", 0);
    quitButton = Button(20, windowHeight - buttonSize - 20, turnButtonWidth, buttonSize, "quit", 0);
    replayButton = Button(windowWidth - turnButtonWidth- 20, windowHeight - buttonSize - 20, turnButtonWidth, buttonSize, "replay", 0);
}

Game::Game(const Game& other)
    : grid(other.grid),
      gameEntities(),
      playerTurn(other.playerTurn),
      entitySelected(other.entitySelected),
      textures(other.textures),
      selectedEntityIndex(other.selectedEntityIndex),
      nbplayers(other.nbplayers),
      turn(other.turn),
      unitButtons(other.unitButtons),
      turnButton(other.turnButton),
      undoButton(other.undoButton),
      quitButton(other.quitButton),
      replayButton(other.replayButton),
      draggedButton(nullptr),
      cameraX(other.cameraX),     
      cameraY(other.cameraY),
      cameraSpeed(other.cameraSpeed),
      hoveredButton(other.hoveredButton)
{
    // Copy players
    for (const auto& player : other.gameEntities.players) {
        gameEntities.players.push_back(std::make_shared<Player>(*player));
    }

    // Copy bandits
    for (const auto& bandit : other.gameEntities.bandits) {
        gameEntities.bandits.push_back(std::make_shared<Bandit>(*bandit));
    }

    // Copy banditCamps
    for (const auto& banditCamp : other.gameEntities.banditCamps) {
        gameEntities.banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
    }

    // Copy treasures
    for (const auto& treasure : other.gameEntities.treasures) {
        gameEntities.treasures.push_back(std::make_shared<Treasure>(*treasure));
    }

    // Copy devils
    for (const auto& devil : other.gameEntities.devils) {
        gameEntities.devils.push_back(std::make_shared<Devil>(*devil));
    }

    // Copy forests
    for (const auto& forest : other.gameEntities.forests) {
        gameEntities.forests.push_back(std::make_shared<Forest>(*forest));
    }
}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        grid = other.grid;
        playerTurn = other.playerTurn;
        entitySelected = other.entitySelected;
        selectedEntityIndex = other.selectedEntityIndex;
        textures = other.textures;
        nbplayers = other.nbplayers;
        turn = other.turn;
        unitButtons = other.unitButtons;
        turnButton = other.turnButton;
        undoButton = other.undoButton;
        quitButton = other.quitButton;
        replayButton = other.replayButton;
        draggedButton = nullptr;
        cameraX = other.cameraX;
        cameraY = other.cameraY;
        cameraSpeed = other.cameraSpeed;

        // Copy players
        gameEntities.players.clear();
        for (const auto& player : other.gameEntities.players) {
            gameEntities.players.push_back(std::make_shared<Player>(*player));
        }

        // Copy bandits
        gameEntities.bandits.clear();
        for (const auto& bandit : other.gameEntities.bandits) {
            gameEntities.bandits.push_back(std::make_shared<Bandit>(*bandit));
        }

        // Copy banditCamps
        gameEntities.banditCamps.clear();
        for (const auto& banditCamp : other.gameEntities.banditCamps) {
            gameEntities.banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
        }

        // Copy treasures
        gameEntities.treasures.clear();
        for (const auto& treasure : other.gameEntities.treasures) {
            gameEntities.treasures.push_back(std::make_shared<Treasure>(*treasure));
        }

        // Copy devils
        gameEntities.devils.clear();
        for (const auto& devil : other.gameEntities.devils) {
            gameEntities.devils.push_back(std::make_shared<Devil>(*devil));
        }

        // Copy forests
        gameEntities.forests.clear();
        for (const auto& forest : other.gameEntities.forests) {
            gameEntities.forests.push_back(std::make_shared<Forest>(*forest));
        }
    }
    return *this;
}

Game::~Game() {
    for (SDL_Texture* texture : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
}

void Game::handleEvent(SDL_Event& event) {
    // if 'E' is pressed or turnbutton clicked, change player
    if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) 
    || (event.type == SDL_MOUSEBUTTONDOWN && turnButton.containsPoint(event.button.x, event.button.y))
    || (event.type == SDL_MOUSEBUTTONDOWN && undoButton.containsPoint(event.button.x, event.button.y))
    || (event.type == SDL_MOUSEBUTTONDOWN && quitButton.containsPoint(event.button.x, event.button.y))
    || (event.type == SDL_MOUSEBUTTONDOWN && replayButton.containsPoint(event.button.x, event.button.y))) {

        if (gameEntities.players.size() == 1
            && event.type == SDL_MOUSEBUTTONDOWN && replayButton.containsPoint(event.button.x, event.button.y)) {
            replayButtonClicked = true;
            return;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && undoButton.containsPoint(event.button.x, event.button.y)) {
            undo = true;
            return;
        }

        if(gameEntities.players.size() == 1 || (event.type == SDL_MOUSEBUTTONDOWN && quitButton.containsPoint(event.button.x, event.button.y))) {
            endGame = true;
            return;
        }

        // if entities on hex not existing on the grid refund the cost of the entity
        if(entitySelected) {
            auto& entity = gameEntities.players[playerTurn]->getEntities()[selectedEntityIndex];
            Hex entityHex = entity->getHex();
            if(!(grid.hexExists(entityHex))) {
                gameEntities.players[playerTurn]->removeEntity(entity);
                // refund the cost of the entity
                for(auto& button : unitButtons) {
                    if(button.getIconName() == entity->getName()) {
                        gameEntities.players[playerTurn]->addCoins(button.getCost());
                    }
                }
            }
        }
        selectedEntityIndex = -1;
        entitySelected = false;

        for(auto& player : gameEntities.players) {
            playerManager.checkIfHexConnectedToTown(*player, grid, gameEntities.bandits, gameEntities.banditCamps);
        }
        

        // Remove dead players
        std::vector<std::shared_ptr<Player>> toRemove;
        for (auto& player : gameEntities.players) {
            if (!player->isAlive()) {
                toRemove.push_back(player);
            }
        }
        for (auto& player : toRemove) {
            playerManager.removePlayer(player, gameEntities.players, nbplayers, gameEntities.bandits, gameEntities.banditCamps);
        }
        // Change player
        playerTurn = (playerTurn + 1) % gameEntities.players.size();
        auto& currentPlayer = gameEntities.players[playerTurn];

        // BANDIT AND TREASURE ACTIONS HERE
        if (playerTurn == 0) {
            turn++;
            if(turn > 0) {
                entityManager.manageBandits(grid, gameEntities);
            }
            if(gameEntities.treasures.empty()) {
                int treasureValue = std::rand() % 10 + 1;
                if(std::rand() % 4 == 0) {
                    Hex treasureHex = entityManager.randomfreeHex(grid, gameEntities);
                    if(treasureHex.getQ() != -1000 && treasureHex.getR() != 0 && treasureHex.getS() != 1000) {
                        entityManager.addTreasure(treasureHex, treasureValue, gameEntities.treasures);
                    }
                }
            }

            if (gameEntities.devils.empty()) {
                if(std::rand() % 1000 == 0) {
                    Hex devilHex = entityManager.randomfreeHex(grid, gameEntities);
                    if(devilHex.getQ() != -1000 && devilHex.getR() != 0 && devilHex.getS() != 1000) {
                        entityManager.addDevil(devilHex, gameEntities.devils);
                        // kill all the entities around the devil
                        for(auto& player : gameEntities.players) {
                            std::vector<std::shared_ptr<Entity>> entitiesToRemove;
                            for(auto& entity : player->getEntities()) {
                                if(entity->getHex().distance(devilHex) <= 1 && entity->getName() != "town" && entity->getProtectionLevel() <= 2) {
                                    entitiesToRemove.push_back(entity);
                                }
                            }
                            for(auto& entity : entitiesToRemove) {
                                player->removeEntity(entity);
                            }
                        }
                        // remove all the bandits around the devil
                        std::vector<std::shared_ptr<Bandit>> banditsToRemove;
                        for(auto& bandit : gameEntities.bandits) {
                            if(bandit->getHex().distance(devilHex) <= 1) {
                                banditsToRemove.push_back(bandit);
                            }
                        }
                        for(auto& bandit : banditsToRemove) {
                            gameEntities.bandits.erase(std::remove(gameEntities.bandits.begin(), gameEntities.bandits.end(), bandit), gameEntities.bandits.end());
                        }
                        // remove all the bandit camps around the devil
                        std::vector<std::shared_ptr<BanditCamp>> banditCampsToRemove;
                        for(auto& banditcamp : gameEntities.banditCamps) {
                            if(banditcamp->getHex().distance(devilHex) <= 1) {
                                banditCampsToRemove.push_back(banditcamp);
                            }
                        }
                        for(auto& banditcamp : banditCampsToRemove) {
                            gameEntities.banditCamps.erase(std::remove(gameEntities.banditCamps.begin(), gameEntities.banditCamps.end(), banditcamp), gameEntities.banditCamps.end());
                        }
                        // remove all the treasures around the devil
                        std::vector<std::shared_ptr<Treasure>> treasuresToRemove;
                        for(auto& treasure : gameEntities.treasures) {
                            if(treasure->getHex().distance(devilHex) <= 1) {
                                treasuresToRemove.push_back(treasure);
                            }
                        }
                        for(auto& treasure : treasuresToRemove) {
                            gameEntities.treasures.erase(std::remove(gameEntities.treasures.begin(), gameEntities.treasures.end(), treasure), gameEntities.treasures.end());
                        }
                    }
                }
            } else {
                // remove all the devils
                for(auto& devil : gameEntities.devils) {
                    gameEntities.devils.erase(std::remove(gameEntities.devils.begin(), gameEntities.devils.end(), devil), gameEntities.devils.end());
                }

            }
        }

        // END OF BANDIT AND TREASURE ACTIONS

        if(turn > 0) {
            // land income
            currentPlayer->addCoins(grid.getNbCasesColor(currentPlayer->getColor()));

            // Pay upkeep for each entity in reverse order (because we remove entities)
            std::vector<std::shared_ptr<Entity>> toRemove;
            for(auto& entity : currentPlayer->getEntities()) {
                bool building = dynamic_cast<Building*>(entity.get());

                if(!building && gameEntities.players.size() > 1) { // if the player is alone, no need to make the units ready to move
                    entity->setMoved(false);
                }
                int currentUpkeep = entity->getUpkeep();

                if(currentPlayer->getCoins() >= currentUpkeep) {
                    currentPlayer->removeCoins(currentUpkeep);
                } else {
                    Hex entityHex = entity->getHex();
                    if(building) {
                        // replace by bandit camp
                        entityManager.addBanditCamp(entityHex, gameEntities.banditCamps);
                    } else {
                        // replace by bandit
                        entityManager.addBandit(entityHex, gameEntities.bandits);
                    }
                    toRemove.push_back(entity);
                }
            }
            for(auto& entity : toRemove) {
                currentPlayer->removeEntity(entity);
            }
        }
    }

    if (event.type == SDL_MOUSEWHEEL) {
        int hexSize = grid.getHexSize();
        int minimumZoom = 10;
        int camCompensation = 9; // value found by trial and error
        if (event.wheel.y > 0) {
            grid.setHexSize(hexSize + 1);
            cameraX += camCompensation;
            cameraY += camCompensation;
        } else if (event.wheel.y < 0 && hexSize > minimumZoom) {
            grid.setHexSize(hexSize - 1);
            cameraX -= camCompensation;
            cameraY -= camCompensation;
        }
    }

    std::string entityToBuy = "";
    if(event.type == SDL_KEYDOWN){
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_q) {
            cameraX -= cameraSpeed;
        } 
        if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
            cameraX += cameraSpeed;
        } 
        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_z) {
            cameraY -= cameraSpeed;
        } 
        if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
            cameraY += cameraSpeed;
        }

        if(event.key.keysym.sym == SDLK_1 || event.key.keysym.sym == SDLK_AMPERSAND) { // AZERTY: '&'
            entityToBuy = "villager";
        }
        if(event.key.keysym.sym == SDLK_2 || event.key.keysym.sym == SDLK_QUOTEDBL) { // AZERTY: 'é'
            entityToBuy = "pikeman";
        }
        if(event.key.keysym.sym == SDLK_3 || event.key.keysym.sym == SDLK_QUOTE) { // AZERTY: '"'
            entityToBuy = "knight";
        }
        if(event.key.keysym.sym == SDLK_4 || event.key.keysym.sym == SDLK_LEFTPAREN) { // AZERTY: '''
            entityToBuy = "hero";
        }
        if(event.key.keysym.sym == SDLK_5 || event.key.keysym.sym == SDLK_MINUS) { // AZERTY: '('
            entityToBuy = "castle";
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN || entityToBuy != "") {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY, cameraX, cameraY);

        if(entityToBuy != "") clickedHex = grid.pixelToHex(0, 0, cameraX, cameraY);

        // Check if a button was clicked
        for (auto& button : unitButtons) {
            if ((button.containsPoint(mouseX, mouseY) || button.getIconName() == entityToBuy)&& !entitySelected) {
                if (button.getCost() <= gameEntities.players[playerTurn]->getCoins()) {
                    if (button.getIconName() == "villager") {
                        gameEntities.players[playerTurn]->addEntity(std::make_shared<Villager>(clickedHex));
                    } else if (button.getIconName() == "pikeman") {
                        gameEntities.players[playerTurn]->addEntity(std::make_shared<Pikeman>(clickedHex));
                    } else if (button.getIconName() == "knight") {
                        gameEntities.players[playerTurn]->addEntity(std::make_shared<Knight>(clickedHex));
                    } else if (button.getIconName() == "hero") {
                        gameEntities.players[playerTurn]->addEntity(std::make_shared<Hero>(clickedHex));
                    } else if(button.getIconName() == "castle") {
                        gameEntities.players[playerTurn]->addEntity(std::make_shared<Castle>(clickedHex));
                        gameEntities.players[playerTurn]->getEntities().back()->setMoved(false);
                    }
                    gameEntities.players[playerTurn]->removeCoins(button.getCost());
                }
            }
        }


        if (playerTurn >= 0 && playerTurn < gameEntities.players.size()) {
            if (!entitySelected) {
                auto& currentPlayer = gameEntities.players[playerTurn];
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
            } else if (entityToBuy == "") {
                bool moveSuccessful = false;
                if (grid.hexExists(clickedHex)) {
                    auto& currentPlayer = gameEntities.players[playerTurn];
                    auto entity = currentPlayer->getEntities()[selectedEntityIndex];
                    SDL_Color targetColor = grid.getHexColors().at(clickedHex);

                    if ((entity) &&
                        !entityManager.isSurroundedByOtherPlayerEntities(clickedHex, *currentPlayer, entity->getProtectionLevel(), grid, gameEntities) &&
                        playerManager.hasSamePlayerEntities(clickedHex, *currentPlayer) == "") {
                        if(entity->getName() == "castle" && !entityManager.entityOnHex(clickedHex, gameEntities) && grid.getHexColors().at(clickedHex) == currentPlayer->getColor()) {
                            entity->setHex(clickedHex);
                            entity->setMoved(true);
                        }

                        moveSuccessful = entity->move(grid, clickedHex, currentPlayer->getColor());

                        // We flag the entity as moved if the move was successful
                        if (moveSuccessful) {
                            bool movedOnSameColor = targetColor == currentPlayer->getColor();

                            // remove potential bandits on the hex we are moving to
                            for (auto& bandit : gameEntities.bandits) {
                                if (bandit->getHex() == clickedHex) {
                                    gameEntities.bandits.erase(std::remove(gameEntities.bandits.begin(), gameEntities.bandits.end(), bandit), gameEntities.bandits.end());
                                    entity->setMoved(true);
                                    break;
                                }
                            }

                            // remove potential bandit camps on the hex we are moving to
                            for (auto& banditcamp : gameEntities.banditCamps) {
                                if (banditcamp->getHex() == clickedHex) {
                                    gameEntities.banditCamps.erase(std::remove(gameEntities.banditCamps.begin(), gameEntities.banditCamps.end(), banditcamp), gameEntities.banditCamps.end());
                                    entity->setMoved(true);
                                    break;
                                }
                            }

                            if(!movedOnSameColor) {
                                entity->setMoved(true);
                                // remove potential entity on the hex we are moving to
                                for (auto& player : gameEntities.players) {
                                    if (player->getColor() == currentPlayer->getColor()) {
                                        continue;
                                    }
                                    for (auto& entity : player->getEntities()) {
                                        if (entity->getHex() == clickedHex) {
                                            if (entity->getName() == "town") {
                                                player->setAlive(false);
                                                int coinsOfDeadPlayer = player->getCoins();
                                                currentPlayer->addCoins(coinsOfDeadPlayer);
                                            }
                                            player->removeEntity(entity);
                                            break;
                                        }
                                    }
                                }
                                // check if a player is on a treasure, give the coins to the player and remove the treasure
                                for(auto& treasure : gameEntities.treasures) {
                                    for (auto& player : gameEntities.players) {
                                        for (auto& entity : player->getEntities()) {
                                            if(treasure->getHex() == entity->getHex()) {
                                                player->addCoins(treasure->getValue());
                                                gameEntities.treasures.erase(std::remove(gameEntities.treasures.begin(), gameEntities.treasures.end(), treasure), gameEntities.treasures.end());
                                                break;
                                            }
                                        }
                                    }
                                }

                                // check if a player beat the devil, give the coins to the player and remove the devil
                                for(auto& devil : gameEntities.devils) {
                                    for (auto& player : gameEntities.players) {
                                        for (auto& entity : player->getEntities()) {
                                            if(devil->getHex() == entity->getHex()) {
                                                player->addCoins(devil->getUpkeep());
                                                gameEntities.devils.erase(std::remove(gameEntities.devils.begin(), gameEntities.devils.end(), devil), gameEntities.devils.end());
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            if(!(grid.hexExists(entity->getHex()))) {
                                currentPlayer->removeEntity(entity);
                                // refund the cost of the entity
                                for(auto& button : unitButtons) {
                                    if(button.getIconName() == entity->getName()) {
                                        currentPlayer->addCoins(button.getCost());
                                    }
                                }
                            }
                        }
                    } else if (entity && !(entity->getHex() == clickedHex) && entity->getName() == playerManager.hasSamePlayerEntities(clickedHex, *currentPlayer) && entity->getName() != "hero" && entity->getName() != "castle") {
                        currentPlayer->removeEntity(entity);
                        entityManager.upgradeEntity(clickedHex, gameEntities.players);
                    } else if(entity && !grid.hexExists(entity->getHex())) {
                        currentPlayer->removeEntity(entity);
                        // refund the cost of the entity
                        for(auto& button : unitButtons) {
                            if(button.getIconName() == entity->getName()) {
                                currentPlayer->addCoins(button.getCost());
                            }
                        }
                    }
                } else {
                    // handle when the hex in question is on a button (then doesnt exist)
                    auto& entity = gameEntities.players[playerTurn]->getEntities()[selectedEntityIndex];
                    Hex entityHex = entity->getHex();
                    if(!(grid.hexExists(entityHex))) {
                        gameEntities.players[playerTurn]->removeEntity(entity);
                        // refund the cost of the entity
                        for(auto& button : unitButtons) {
                            if(button.getIconName() == entity->getName()) {
                                gameEntities.players[playerTurn]->addCoins(button.getCost());
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
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && turnButton.containsPoint(event.button.x, event.button.y)) {
        turnButtonClicked = true;
        return;
    }

    if (event.type == SDL_MOUSEMOTION) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        buttonHovered = false;
        for(auto unitButton : unitButtons) {
            if(unitButton.containsPoint(mouseX, mouseY)) {
                buttonHovered = true;
                hoveredButton = unitButton;
            }
        }
    }
}

void Game::update() {
    const float initjumpSpeed = 0.25f;
    const float maxJumpHeight = 5.0f;
    const float minJumpHeight = 0.0f;
    const float jumpSpeedDecrease = initjumpSpeed / 50.0f;
    const float jumpSpeedIncrease = jumpSpeedDecrease;

    for (auto& player : gameEntities.players) {
        if (player != gameEntities.players[playerTurn]) {
            for (auto& entity : player->getEntities()) {
                if (!entity->hasMoved() && !dynamic_cast<Building*>(entity.get())) {
                    entity->setYOffset(minJumpHeight);
                    entity->setJumping(false);
                    entity->setFalling(false);
                    entity->setJumpSpeed(initjumpSpeed);
                }
            }
            continue;
        }
        float syncY = 0.0f;
        bool foundSync = false;

        for (auto& entity : player->getEntities()) {
            if (entity->hasMoved() || dynamic_cast<Building*>(entity.get())) {
                entity->setYOffset(minJumpHeight);
                entity->setJumping(false);
                entity->setFalling(false);
                entity->setJumpSpeed(initjumpSpeed);
                if (entity->getYOffset() > minJumpHeight) {
                    entity->setYOffset(minJumpHeight);
                }
                continue;
            }

            if (!foundSync) {
                foundSync = true;

                if (!entity->isJumping() && !entity->isFalling()) {
                    entity->setJumping(true);
                    entity->setJumpSpeed(initjumpSpeed);
                }

                if (entity->isJumping()) {
                    entity->setYOffset(entity->getYOffset() + entity->getJumpSpeed());
                    entity->setJumpSpeed(entity->getJumpSpeed() - jumpSpeedDecrease);

                    if (entity->getYOffset() > maxJumpHeight) {
                        entity->setJumping(false);
                        entity->setFalling(true);
                        entity->setYOffset(maxJumpHeight);
                    }
                } else if (entity->isFalling()) {
                    entity->setJumpSpeed(std::min(entity->getJumpSpeed() + jumpSpeedIncrease, initjumpSpeed));
                    entity->setYOffset(entity->getYOffset() - entity->getJumpSpeed());

                    if (entity->getYOffset() <= minJumpHeight) {
                        entity->setFalling(false);
                        entity->setJumping(true);
                        entity->setYOffset(minJumpHeight);
                    }
                }

                syncY = entity->getYOffset();
            } else {
                entity->setYOffset(syncY);
            }
        }
    }
}

void Game::renderAll(SDL_Renderer* renderer) const {
    // Draw the grid
    grid.draw(renderer, cameraX, cameraY);

    // Render all entities
    renderGame.renderEntities(renderer, gameEntities.bandits, "bandit", grid, cameraX, cameraY, textures);
    renderGame.renderEntities(renderer, gameEntities.banditCamps, "bandit-camp", grid, cameraX, cameraY, textures);
    renderGame.renderEntities(renderer, gameEntities.treasures, "treasury", grid, cameraX, cameraY, textures);
    renderGame.renderEntities(renderer, gameEntities.devils, "devil", grid, cameraX, cameraY, textures);
    renderGame.renderEntities(renderer, gameEntities.forests, "forest", grid, cameraX, cameraY, textures);

    // Render all players' entities
    renderGame.renderPlayersEntities(renderer, gameEntities.players, playerTurn, grid, cameraX, cameraY, textures, entitySelected, selectedEntityIndex);

    // Render the selected entity if any
    renderGame.renderSelectedEntity(renderer, playerTurn, grid, gameEntities, cameraX, cameraY, textures, entitySelected, selectedEntityIndex);

    // Display current player's color and information
    renderGame.renderPlayerInfo(renderer, gameEntities.players, playerTurn, grid, textures);

    // Render all buttons
    renderGame.renderAllButtons(renderer, unitButtons, textures, gameEntities.players, playerTurn, turnButton, undoButton, quitButton, replayButton);

    // Display game over message if only one player remains
    renderGame.renderGameOverMessage(renderer, gameEntities.players, textures, unitButtons);

    if(buttonHovered && !entitySelected) {
        renderGame.RenderButtonInfo(renderer, hoveredButton, textures);
    }
}
