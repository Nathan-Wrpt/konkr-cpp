#ifndef GAME_HPP
#define GAME_HPP

#include <filesystem>
#include <SDL2/SDL_image.h>

#include "../players/playermanager.hpp"
#include "rendergame.hpp"

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

  bool getEndGame() const { return endGame; }
  void setEndGame(bool endGame) { this->endGame = endGame; }

  bool getUndo() const { return undo; }
  void setUndo(bool undo) { this->undo = undo; }

  bool getTurnButtonClicked() const { return turnButtonClicked; }
  void setTurnButtonClicked(bool clicked) { turnButtonClicked = clicked; }

  bool getReplayButtonClicked() const { return replayButtonClicked; }
  void setReplayButtonClicked(bool clicked) { replayButtonClicked = clicked; }

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
  Button replayButton;
  Button* draggedButton;
  int cameraX, cameraY, cameraSpeed;
  bool endGame;
  bool undo;
  bool turnButtonClicked;
  bool replayButtonClicked;
  bool buttonHovered;
  Button hoveredButton;
  int defaultHexSize;
};

#endif // GAME_HPP
