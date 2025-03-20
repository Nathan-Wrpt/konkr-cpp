#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

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
         int windowWidth, int windowHeight, SDL_Renderer* renderer);
    ~Game();

    void handleEvent(SDL_Event& event);
    void update();
    SDL_Rect entityToRect(const Entity& entity) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture) const;
    void render(SDL_Renderer* renderer) const;
    bool isSurroundedByOtherPlayerEntities(const Hex& hex, const Player& currentPlayer, const int& currentLevel);
    std::string hasSamePlayerEntities(const Hex& hex, const Player& currentPlayer) const;
    void addBandit(Hex hex);
    bool entityOnHex(const Hex& hex);
    void manageBandits();
    int nbBanditsOnColor(const SDL_Color& color);
    void upgradeEntity(const Hex& hex);
    void renderButton(SDL_Renderer* renderer, const Button& button) const;
    void removePlayer(std::shared_ptr<Player> player);

  private:
    HexagonalGrid grid;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Bandit>> bandits;
    size_t playerTurn;
    std::mutex playersMutex;
    bool entitySelected;
    std::vector<SDL_Texture*> textures;
    int selectedEntityIndex;
    int nbplayers;
    int turn;
    std::vector<Button> unitButtons;
    Button* draggedButton;
};

#endif
