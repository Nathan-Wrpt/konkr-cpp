#ifndef RENDERGAME_HPP
#define RENDERGAME_HPP

#include "../entities/entity.hpp"

class RenderGame {
public:
    SDL_Rect entityToRect(const Entity& entity, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture, const HexagonalGrid& grid, int cameraX, int cameraY) const;
};

#endif // RENDERGAME_HPP