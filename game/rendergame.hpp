#ifndef RENDERGAME_HPP
#define RENDERGAME_HPP

#include "../entities/entitymanager.hpp"
#include "../ui/button.hpp"
#include "../ui/data.hpp"

class RenderGame {
public:
    template <typename T>
    void renderEntities(SDL_Renderer* renderer, const std::vector<std::shared_ptr<T>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;
    void renderPlayersEntities(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures, bool entitySelected, int selectedEntityIndex) const;
    void renderSelectedEntity(SDL_Renderer* renderer, size_t playerTurn, const HexagonalGrid& grid, const GameEntities& gameEntities, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures, bool entitySelected, int selectedEntityIndex) const;
    void renderPlayerInfo(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const HexagonalGrid& grid, const std::vector<SDL_Texture*>& textures) const;
    void renderAllButtons(SDL_Renderer* renderer, const std::vector<Button>& unitButtons, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, const int& nbplayers, size_t playerTurn, const Button& turnButton, const Button& undoButton, const Button& quitButton, const Button& replayButton) const;
    void renderTurnButton(SDL_Renderer* renderer, const Button& turnButton, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn) const;
    void RenderButtonInfo(SDL_Renderer* renderer, Button button, const std::vector<SDL_Texture*>& textures) const;
    void renderGameOverMessage(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, const std::vector<SDL_Texture*>& textures, const std::vector<Button>& unitButtons) const;

private:
    SDL_Rect entityToRect(const Entity& entity, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void renderButton(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const;
    void renderButtonNoBorder(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const;
    void renderButtonText(SDL_Renderer* renderer, const Button& button, SDL_Rect& buttonRect, const std::vector<SDL_Texture*>& textures) const;
    SDL_Texture* determineTurnButtonTexture(const std::vector<SDL_Texture*>& textures, const Button& turnButton, const std::shared_ptr<Player>& currentPlayer, const std::vector<std::shared_ptr<Player>>& players) const;

    void highlightAccessibleHexes(SDL_Renderer* renderer, const std::shared_ptr<Entity>& selectedEntity, const HexagonalGrid& grid, int cameraX, int cameraY, size_t playerTurn, const GameEntities& GameEntities, const std::vector<SDL_Texture*>& textures) const;
    void drawHexHighlight(SDL_Renderer* renderer, const Hex& hex, const HexagonalGrid& grid, int cameraX, int cameraY, const GameEntities& gameEntities, const std::vector<SDL_Texture*>& textures) const;

    void renderPlayerResources(SDL_Renderer* renderer, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const std::vector<SDL_Texture*>& textures) const;
    
    EntityManager entityManager;
};

#endif // RENDERGAME_HPP