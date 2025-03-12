#ifndef GAME_HPP
#define GAME_HPP

#include "../core/grid.hpp"
#include "../entities/entity.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Game {
private:
    HexagonalGrid grid;
    Bandit bandit;
    Villager villager;
    SDL_Texture* banditTexture;
    SDL_Texture* villagerTexture;
    bool playerTurn;
    bool villagerSelected;
    int nbplayers;

public:
    Game(double hexSize, const std::vector<std::string>& asciiMap, int windowWidth, int windowHeight, SDL_Renderer* renderer);
    ~Game();
    void handleEvent(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer) const;
};

#endif // GAME_HPP