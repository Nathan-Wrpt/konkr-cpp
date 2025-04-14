#include "game.hpp"
#include <random>
#include <map>
#include <filesystem>
#include <algorithm>
#include <queue>
#include <set>

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
    draggedButton(nullptr),
    cameraSpeed(cameraSpeed),
    endGame(false)
{
    std::cout << "Game constructor started" << std::endl;
    turn = 0;

    cameraX = 0;
    cameraY = 0;
    // Load all textures from the icons directory
    std::cout << "Loading textures..." << std::endl;
    std::string iconsPath = "icons/";
    for (const auto& filename : iconsMap) {
        std::string path = iconsPath + filename.first + ".png";
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
            players.emplace_back(std::make_shared<Player>(color));
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
    entityManager.generateEntities(entityMap, asciiMap, grid, players, bandits, banditCamps, treasures);

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
    turnButton = Button(windowWidth - turnButtonWidth- 20, windowHeight - buttonSize - 20, turnButtonWidth, buttonSize, "zznext", 0);
}

Game::Game(const Game& other)
    : grid(other.grid),
      players(),
      bandits(),
      banditCamps(),
      treasures(),
      playerTurn(other.playerTurn),
      entitySelected(other.entitySelected),
      textures(other.textures),
      selectedEntityIndex(other.selectedEntityIndex),
      nbplayers(other.nbplayers),
      turn(other.turn),
      unitButtons(other.unitButtons),
      turnButton(other.turnButton),
      draggedButton(nullptr),
      cameraX(other.cameraX),     
      cameraY(other.cameraY),
      cameraSpeed(other.cameraSpeed)
{
    // Copy players
    for (const auto& player : other.players) {
        players.push_back(std::make_shared<Player>(*player));
    }

    // Copy bandits
    for (const auto& bandit : other.bandits) {
        bandits.push_back(std::make_shared<Bandit>(*bandit));
    }

    // Copy banditCamps
    for (const auto& banditCamp : other.banditCamps) {
        banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
    }

    // Copy treasures
    for (const auto& treasure : other.treasures) {
        treasures.push_back(std::make_shared<Treasure>(*treasure));
    }

    // Copy devils
    for (const auto& devil : other.devils) {
        devils.push_back(std::make_shared<Devil>(*devil));
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
        draggedButton = nullptr;
        cameraX = other.cameraX;
        cameraY = other.cameraY;
        cameraSpeed = other.cameraSpeed;

        // Copy players
        players.clear();
        for (const auto& player : other.players) {
            players.push_back(std::make_shared<Player>(*player));
        }

        // Copy bandits
        bandits.clear();
        for (const auto& bandit : other.bandits) {
            bandits.push_back(std::make_shared<Bandit>(*bandit));
        }

        // Copy banditCamps
        banditCamps.clear();
        for (const auto& banditCamp : other.banditCamps) {
            banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
        }

        // Copy treasures
        treasures.clear();
        for (const auto& treasure : other.treasures) {
            treasures.push_back(std::make_shared<Treasure>(*treasure));
        }

        // Copy devils
        devils.clear();
        for (const auto& devil : other.devils) {
            devils.push_back(std::make_shared<Devil>(*devil));
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

Hex Game::randomfreeHex() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, grid.getHexes().size() - 1);
    int randomIndex = distrib(gen);
    Hex randomHex = grid.getHexes()[randomIndex];
    SDL_Color hexColor = grid.getHexColors().at(randomHex);
    std::vector<SDL_Color> playerColors;
    for(auto& player : players) {
        playerColors.push_back(player->getColor());
    }
    // check that the Hex is not occupated and not owned by a player
    int maxAttempts = 100;
    int attempts = 0;
    while(entityManager.entityOnHex(randomHex, bandits, banditCamps, treasures, devils, players) || (std::find(playerColors.begin(), playerColors.end(), hexColor) != playerColors.end())) {
        randomIndex = distrib(gen);
        randomHex = grid.getHexes()[randomIndex];
        hexColor = grid.getHexColors().at(randomHex);
        attempts++;
        if(attempts >= maxAttempts) {
            return Hex(-1000, 0, 1000);
        }
    }
    return randomHex;
}

void Game::handleEvent(SDL_Event& event) {
    // if 'E' is pressed or turnbutton clicked, change player
    if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) || (event.type == SDL_MOUSEBUTTONDOWN && turnButton.containsPoint(event.button.x, event.button.y))) {
        if(players.size() == 1) {
            endGame = true;
            return;
        }
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
        

        // Remove dead players
        std::vector<std::shared_ptr<Player>> toRemove;
        for (auto& player : players) {
            if (!player->isAlive()) {
                toRemove.push_back(player);
            }
        }
        for (auto& player : toRemove) {
            removePlayer(player);
        }
        // Change player
        playerTurn = (playerTurn + 1) % players.size();
        auto& currentPlayer = players[playerTurn];

        // BANDIT AND TREASURE ACTIONS HERE
        if (playerTurn == 0) {
            turn++;
            if(turn > 0) {
                entityManager.manageBandits(grid, bandits, banditCamps, treasures, devils, players);
            }
            if(treasures.empty()) {
                int treasureValue = std::rand() % 10 + 1;
                if(std::rand() % 4 == 0) {
                    Hex treasureHex = randomfreeHex();
                    if(treasureHex.getQ() != -1000 && treasureHex.getR() != 0 && treasureHex.getS() != 1000) {
                        entityManager.addTreasure(treasureHex, treasureValue, treasures);
                    }
                }
            }

            if (devils.empty()) {
                if(std::rand() % 1000 == 0) {
                    Hex devilHex = randomfreeHex();
                    if(devilHex.getQ() != -1000 && devilHex.getR() != 0 && devilHex.getS() != 1000) {
                        entityManager.addDevil(devilHex, devils);
                        // kill all the entities around the devil
                        for(auto& player : players) {
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
                        for(auto& bandit : bandits) {
                            if(bandit->getHex().distance(devilHex) <= 1) {
                                banditsToRemove.push_back(bandit);
                            }
                        }
                        for(auto& bandit : banditsToRemove) {
                            bandits.erase(std::remove(bandits.begin(), bandits.end(), bandit), bandits.end());
                        }
                        // remove all the bandit camps around the devil
                        std::vector<std::shared_ptr<BanditCamp>> banditCampsToRemove;
                        for(auto& banditcamp : banditCamps) {
                            if(banditcamp->getHex().distance(devilHex) <= 1) {
                                banditCampsToRemove.push_back(banditcamp);
                            }
                        }
                        for(auto& banditcamp : banditCampsToRemove) {
                            banditCamps.erase(std::remove(banditCamps.begin(), banditCamps.end(), banditcamp), banditCamps.end());
                        }
                        // remove all the treasures around the devil
                        std::vector<std::shared_ptr<Treasure>> treasuresToRemove;
                        for(auto& treasure : treasures) {
                            if(treasure->getHex().distance(devilHex) <= 1) {
                                treasuresToRemove.push_back(treasure);
                            }
                        }
                        for(auto& treasure : treasuresToRemove) {
                            treasures.erase(std::remove(treasures.begin(), treasures.end(), treasure), treasures.end());
                        }
                    }
                }
            } else {
                // remove all the devils
                for(auto& devil : devils) {
                    devils.erase(std::remove(devils.begin(), devils.end(), devil), devils.end());
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

                if(!building && players.size() > 1) { // if the player is alone, no need to make the units ready to move
                    entity->setMoved(false);
                }
                int currentUpkeep = entity->getUpkeep();

                if(currentPlayer->getCoins() >= currentUpkeep) {
                    currentPlayer->removeCoins(currentUpkeep);
                } else {
                    Hex entityHex = entity->getHex();
                    if(building) {
                        // replace by bandit camp
                        entityManager.addBanditCamp(entityHex, banditCamps);
                    } else {
                        // replace by bandit
                        entityManager.addBandit(entityHex, bandits);
                    }
                    toRemove.push_back(entity);
                }
            }
            for(auto& entity : toRemove) {
                currentPlayer->removeEntity(entity);
            }
        }
    }

    if (event.key.keysym.sym == SDLK_LEFT) {
        cameraX -= cameraSpeed;
    } 
    if (event.key.keysym.sym == SDLK_RIGHT) {
        cameraX += cameraSpeed;
    } 
    if (event.key.keysym.sym == SDLK_UP) {
        cameraY -= cameraSpeed;
    } 
    if (event.key.keysym.sym == SDLK_DOWN) {
        cameraY += cameraSpeed;
    }
    std::string entityToBuy = "";

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

    if (event.type == SDL_MOUSEBUTTONDOWN || entityToBuy != "") {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY, cameraX, cameraY);

        if(entityToBuy != "") clickedHex = grid.pixelToHex(0, 0, cameraX, cameraY);

        // Check if a button was clicked
        for (auto& button : unitButtons) {
            if ((button.containsPoint(mouseX, mouseY) || button.getIconName() == entityToBuy)&& !entitySelected) {
                if (button.getCost() <= players[playerTurn]->getCoins()) {
                    if (button.getIconName() == "villager") {
                        players[playerTurn]->addEntity(std::make_shared<Villager>(clickedHex));
                    } else if (button.getIconName() == "pikeman") {
                        players[playerTurn]->addEntity(std::make_shared<Pikeman>(clickedHex));
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
            } else if (entityToBuy == "") {
                bool moveSuccessful = false;
                if (grid.hexExists(clickedHex)) {
                    auto& currentPlayer = players[playerTurn];
                    auto entity = currentPlayer->getEntities()[selectedEntityIndex];
                    SDL_Color targetColor = grid.getHexColors().at(clickedHex);

                    if ((entity) &&
                        !entityManager.isSurroundedByOtherPlayerEntities(clickedHex, *currentPlayer, entity->getProtectionLevel(), grid, players, banditCamps, devils) &&
                        hasSamePlayerEntities(clickedHex, *currentPlayer) == "") {
                        if(entity->getName() == "castle" && !entityManager.entityOnHex(clickedHex, bandits, banditCamps, treasures, devils, players) && grid.getHexColors().at(clickedHex) == currentPlayer->getColor()) {
                            entity->setHex(clickedHex);
                            entity->setMoved(true);
                        }

                        moveSuccessful = entity->move(grid, clickedHex, currentPlayer->getColor());

                        // We flag the entity as moved if the move was successful
                        if (moveSuccessful) {
                            bool movedOnSameColor = targetColor == currentPlayer->getColor();

                            // remove potential bandits on the hex we are moving to
                            for (auto& bandit : bandits) {
                                if (bandit->getHex() == clickedHex) {
                                    bandits.erase(std::remove(bandits.begin(), bandits.end(), bandit), bandits.end());
                                    entity->setMoved(true);
                                    break;
                                }
                            }

                            // remove potential bandit camps on the hex we are moving to
                            for (auto& banditcamp : banditCamps) {
                                if (banditcamp->getHex() == clickedHex) {
                                    banditCamps.erase(std::remove(banditCamps.begin(), banditCamps.end(), banditcamp), banditCamps.end());
                                    entity->setMoved(true);
                                    break;
                                }
                            }

                            if(!movedOnSameColor) {
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
                                                int coinsOfDeadPlayer = player->getCoins();
                                                currentPlayer->addCoins(coinsOfDeadPlayer);
                                            }
                                            player->removeEntity(entity);
                                            break;
                                        }
                                    }
                                }
                                // check if a player is on a treasure, give the coins to the player and remove the treasure
                                for(auto& treasure : treasures) {
                                    for (auto& player : players) {
                                        for (auto& entity : player->getEntities()) {
                                            if(treasure->getHex() == entity->getHex()) {
                                                player->addCoins(treasure->getValue());
                                                treasures.erase(std::remove(treasures.begin(), treasures.end(), treasure), treasures.end());
                                                break;
                                            }
                                        }
                                    }
                                }

                                // check if a player beat the devil, give the coins to the player and remove the devil
                                for(auto& devil : devils) {
                                    for (auto& player : players) {
                                        for (auto& entity : player->getEntities()) {
                                            if(devil->getHex() == entity->getHex()) {
                                                player->addCoins(devil->getUpkeep());
                                                devils.erase(std::remove(devils.begin(), devils.end(), devil), devils.end());
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
                    } else if (entity && !(entity->getHex() == clickedHex) && entity->getName() == hasSamePlayerEntities(clickedHex, *currentPlayer) && entity->getName() != "hero" && entity->getName() != "castle") {
                        currentPlayer->removeEntity(entity);
                        entityManager.upgradeEntity(clickedHex, players);
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
    }
}

void Game::update() {
    const float initjumpSpeed = 0.25f;
    const float maxJumpHeight = 5.0f;
    const float minJumpHeight = 0.0f;
    const float jumpSpeedDecrease = initjumpSpeed / 50.0f;
    const float jumpSpeedIncrease = jumpSpeedDecrease;

    for (auto& player : players) {
        if (player != players[playerTurn]) {
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
    renderGame.render(renderer, grid, bandits, banditCamps, treasures, devils, players, playerTurn, cameraX, cameraY, textures, unitButtons, turnButton, entitySelected, selectedEntityIndex);
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


void Game::checkIfHexConnectedToTown(Player& player) {
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
                entityManager.addBanditCamp(hex, banditCamps);
            } else {
                entityManager.addBandit(hex, bandits);
            }
            player.removeEntity(entity);
            break;
        }
    }
}
