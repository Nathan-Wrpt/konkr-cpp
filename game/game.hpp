#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../entities/building.hpp"
#include "../players/player.hpp"
#include "../ui/button.hpp"
#include "../ui/data.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

extern std::map<std::string, int> iconsMap;

class Game {
  public:
    Game(double hexSize, const std::vector<std::string>& asciiMap,
         int windowWidth, int windowHeight, SDL_Renderer* renderer, int cameraSpeed);
    ~Game();

    // Copy constructor
    Game(const Game& other) :
        grid(other.grid),
        players(),
        bandits(),
        banditCamps(),
        textures(other.textures),
        playerTurn(other.playerTurn),
        entitySelected(other.entitySelected),
        selectedEntityIndex(other.selectedEntityIndex),
        nbplayers(other.nbplayers),
        turn(other.turn),
        unitButtons(other.unitButtons),
        draggedButton(nullptr),
        cameraX(other.cameraX),
        cameraY(other.cameraY),
        cameraSpeed(other.cameraSpeed)
    {

        //  copy players
        for (const auto& player : other.players) {
            players.push_back(std::make_shared<Player>(*player));
        }

        //  copy bandits
        for (const auto& bandit : other.bandits) {
            bandits.push_back(std::make_shared<Bandit>(*bandit));
        }

        // copy banditCamps
        for (const auto& banditCamp : other.banditCamps) {
            banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
        }

    }

    // Assignment operator
    Game& operator=(const Game& other) {
        if (this != &other) {
            grid = other.grid;
            playerTurn = other.playerTurn;
            entitySelected = other.entitySelected;
            selectedEntityIndex = other.selectedEntityIndex;
            textures = other.textures;
            nbplayers = other.nbplayers;
            turn = other.turn;
            unitButtons = other.unitButtons;
            draggedButton = nullptr;
            cameraX = other.cameraX;
            cameraY = other.cameraY;
            cameraSpeed = other.cameraSpeed;

            // copy players
            players.clear();
            for (const auto& player : other.players) {
                players.push_back(std::make_shared<Player>(*player));
            }

            // copy bandits
            bandits.clear();
            for (const auto& bandit : other.bandits) {
                bandits.push_back(std::make_shared<Bandit>(*bandit));
            }

            // copy banditCamps
            banditCamps.clear();
            for (const auto& banditCamp : other.banditCamps) {
                banditCamps.push_back(std::make_shared<BanditCamp>(*banditCamp));
            }
        }
        return *this;
    }

    void handleEvent(SDL_Event& event);
    void update();
    SDL_Rect entityToRect(const Entity& entity) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture) const;
    void render(SDL_Renderer* renderer) const;
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel);
    std::string hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const;
    void addBandit(Hex hex);
    void addBanditCamp(Hex hex);
    void addTreasure(Hex hex, int value);
    bool entityOnHex(const Hex& hex);
    void manageBandits();
    int nbBanditsOnColor(const SDL_Color& color);
    void upgradeEntity(const Hex& hex);
    void renderButton(SDL_Renderer* renderer, const Button& button) const;
    void removePlayer(std::shared_ptr<Player> player);
    void checkIfHexConnectedToTown(Player& player);
    void disconnectHex(Player& player, const Hex& hex);
    Hex randomfreeHex();

  private:
    HexagonalGrid grid;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Bandit>> bandits;
    std::vector<std::shared_ptr<BanditCamp>> banditCamps;
    std::vector<std::shared_ptr<Treasure>> treasures;
    size_t playerTurn;
    bool entitySelected;
    std::vector<SDL_Texture*> textures;
    int selectedEntityIndex;
    int nbplayers;
    int turn;
    std::vector<Button> unitButtons;
    Button* draggedButton;
    int cameraX, cameraY, cameraSpeed;
};

#endif
