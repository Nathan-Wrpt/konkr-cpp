#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "playermanager.hpp"
#include "rendergame.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class Game {
  public:
    Game(double hexSize, const std::vector<std::string>& asciiMap, std::vector<std::string>& entityMap,
         int windowWidth, int windowHeight, SDL_Renderer* renderer, int cameraSpeed);
    ~Game();

    // Copy constructor
    Game(const Game& other);

    // Assignment operator
    Game& operator=(const Game& other);

    void handleEvent(SDL_Event& event);
    void update();
    void renderAll(SDL_Renderer* renderer) const;
    std::string hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const;
    Hex randomfreeHex();
    bool isEndGame() const { return endGame; }

  private:
    HexagonalGrid grid;
    EntityManager entityManager;
    RenderGame renderGame;
    PlayerManager playerManager;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Bandit>> bandits;
    std::vector<std::shared_ptr<BanditCamp>> banditCamps;
    std::vector<std::shared_ptr<Treasure>> treasures;
    std::vector<std::shared_ptr<Devil>> devils;
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
    bool endGame;
};

#endif // GAME_HPP
