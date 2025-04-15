#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "../players/playermanager.hpp"
#include "rendergame.hpp"
#include "gameentities.hpp"

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
  bool isEndGame() const { return endGame; }

  bool getUndo() const { return undo; }
  void setUndo(bool undo) { this->undo = undo; }

private:
  HexagonalGrid grid;
  EntityManager entityManager;
  RenderGame renderGame;
  PlayerManager playerManager;
  GameEntities gameEntities;
  size_t playerTurn;
  bool entitySelected;
  std::vector<SDL_Texture*> textures;
  int selectedEntityIndex;
  int nbplayers;
  int turn;
  std::vector<Button> unitButtons;
  Button turnButton;
  Button undoButton;
  Button quitButton;
  Button* draggedButton;
  int cameraX, cameraY, cameraSpeed;
  bool endGame;
  bool undo;
};

#endif // GAME_HPP
