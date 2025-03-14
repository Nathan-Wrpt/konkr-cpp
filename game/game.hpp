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
