#ifndef RENDERGAME_HPP
#define RENDERGAME_HPP

#include <SDL2/SDL.h>
#include <string>

#include "../constants/constants.hpp"
#include "../game/game.hpp"

class RenderGame {
public:
    RenderGame(SDL_Renderer* renderer, const Game& game);

    void renderEntity(const class Entity& entity, SDL_Texture* texture) const;
    void renderButton(const class Button& button) const;
    void renderPlayers() const;
    void renderGrid() const;
    void renderPlayerInfo() const;
    void renderButtons() const;
    void renderSelectedEntity() const;
    void drawButtonCostText(const class Button& button, const SDL_Rect& buttonRect) const;
    void renderAll() const;

private:
    SDL_Renderer* renderer;
    const Game& game;
};

#endif // RENDERGAME_HPP