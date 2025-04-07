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
    Game(double hexSize, const std::vector<std::string>& asciiMap, std::vector<std::string>& entityMap,
         int windowWidth, int windowHeight, SDL_Renderer* renderer, int cameraSpeed);
    ~Game();

    // Copy constructor
    Game(const Game& other);

    // Assignment operator
    Game& operator=(const Game& other);

    void generateEntities(const std::vector<std::string>& entityMap, const std::vector<std::string>& asciiMap);
    void handleEvent(SDL_Event& event);
    void update();
    SDL_Rect entityToRect(const Entity& entity) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture) const;
    void render(SDL_Renderer* renderer) const;
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel) const;
    std::string hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const;
    void addBandit(Hex hex);
    void addBanditCamp(Hex hex);
    void addTreasure(Hex hex, int value);
    bool entityOnHex(const Hex& hex) const;
    void manageBandits();
    int nbBanditsOnColor(const SDL_Color& color);
    void upgradeEntity(const Hex& hex);
    void renderButton(SDL_Renderer* renderer, const Button& button) const;
    void renderTurnButton(SDL_Renderer* renderer) const;
    void removePlayer(std::shared_ptr<Player> player);
    void checkIfHexConnectedToTown(Player& player);
    void disconnectHex(Player& player, const Hex& hex);
    Hex randomfreeHex();
    bool HexNotOnTerritoryAndAccessible(const std::shared_ptr<Entity>& entity, const Hex& targetHex) const;

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
    Button turnButton;
    Button* draggedButton;
    int cameraX, cameraY, cameraSpeed;
};

#endif
