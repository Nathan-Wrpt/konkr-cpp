#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include "../players/player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

std::map<std::string, int> iconsMap = {
  {"bandit", 0},   {"bandit-camp", 1},
  {"castle", 2},        {"coin", 3},
  {"coins", 4},         {"deficit", 5},
  {"emoji-happy", 6},   {"face", 7},
  {"gold-trophy", 8},   {"grave", 9},
  {"hero", 10},        {"knight", 11},
  {"pikeman", 12},     {"silver-trophy", 13},
  {"surplus", 14},      {"town", 15},
  {"treasury", 16},     {"upkeep", 17},
  {"villager", 18}};

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

  private:
    HexagonalGrid grid;
    std::vector<Player> players;
    std::vector<Bandit> bandits;
    std::vector<Villager> villagers;
    bool playerTurn;
    bool villagerSelected;
    std::vector<SDL_Texture*> textures;
    SDL_Texture* banditTexture;
    SDL_Texture* villagerTexture;
    int nbplayers;
    int villagerSelectedIndex;
};

#endif
