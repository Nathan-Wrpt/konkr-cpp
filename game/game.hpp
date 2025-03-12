#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Game {
  public:
    Game(double hexSize, const std::vector<std::string>& asciiMap,
         int windowWidth, int windowHeight, SDL_Renderer* renderer);
    ~Game();

    void handleEvent(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer) const;

  private:
    HexagonalGrid grid;
    Bandit bandit;
    std::vector<Villager> villagers;
    bool playerTurn;
    bool villagerSelected;
    SDL_Texture* banditTexture;
    std::vector<SDL_Texture*> villagerTextures;
    int nbplayers;
    int villagerSelectedIndex;
};

#endif
