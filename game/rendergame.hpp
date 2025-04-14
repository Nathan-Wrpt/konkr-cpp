#ifndef RENDERGAME_HPP
#define RENDERGAME_HPP

#include <SDL2/SDL_ttf.h>

#include "../constants/constants.hpp"
#include "../entities/entity.hpp"
#include "../players/player.hpp"
#include "../ui/button.hpp"

class RenderGame {
public:
    SDL_Rect entityToRect(const Entity& entity, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void renderButton(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const;
    void renderTurnButton(SDL_Renderer* renderer, const Button& turnButton, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn) const;
};

#endif // RENDERGAME_HPP