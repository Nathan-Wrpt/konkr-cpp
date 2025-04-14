#include "rendergame.hpp"

SDL_Rect RenderGame::entityToRect(const Entity& entity, const HexagonalGrid& grid, int cameraX, int cameraY) const {
    int x = static_cast<int>(grid.hexToPixel(entity.getHex()).x - grid.getHexSize() / 2) - cameraX;
    int y = static_cast<int>(grid.hexToPixel(entity.getHex()).y - grid.getHexSize() / 2 - entity.getYOffset()) - cameraY;
    int w_h = static_cast<int>(grid.getHexSize());
    return {x, y, w_h, w_h};
}

void RenderGame::render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture, const HexagonalGrid& grid, int cameraX, int cameraY) const {
    SDL_Rect entityRect = entityToRect(entity, grid, cameraX, cameraY);
    SDL_RenderCopy(renderer, texture, NULL, &entityRect);
}