#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../players/player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

extern std::map<std::string, int> iconsMap;

class Game {
  public:
    Game(double hexSize, const std::vector<std::string>& asciiMap,
         int windowWidth, int windowHeight, SDL_Renderer* renderer);
    ~Game();

    void handleEvent(SDL_Event& event);
    void update();
    SDL_Rect entityToRect(const Entity& entity) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture) const;
    void render(SDL_Renderer* renderer) const;
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel);
    bool isSurroundedBySamePlayerEntities(const Hex& hex, const Player& currentPlayer) const;
    void addBandit(Hex hex);

  private:
    HexagonalGrid grid;
    std::vector<Player> players;
    std::vector<std::shared_ptr<Bandit>> bandits;
    size_t playerTurn;
    bool entitySelected;
    std::vector<SDL_Texture*> textures;
    int selectedEntityIndex;
    int nbplayers;
    int turn;
};

#endif
