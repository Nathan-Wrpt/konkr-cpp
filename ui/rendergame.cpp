#include "rendergame.hpp"

RenderGame ::RenderGame(SDL_Renderer* renderer, const Game& game)
    : renderer(renderer), game(game) {}

void RenderGame::renderEntity(const Entity& entity, SDL_Texture* texture) const {
    SDL_Rect entityRect = game.entityToRect(entity);
    SDL_RenderCopy(renderer, texture, NULL, &entityRect);
}

void RenderGame::renderButton(const Button& button) const {
    SDL_Rect buttonRect = button.getRect();

    // Draw button background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    // Draw button icon
    SDL_Texture* iconTexture = game.getTextures().at(iconsMap.at(button.getIconName()));
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);

    // Draw cost text
    drawButtonCostText(button, buttonRect);
}

void RenderGame::renderPlayers() const {
    const auto& currentPlayer = game.getPlayers()[game.getPlayerTurn()];
    printf("    current player found...\n");
    for (size_t i = 0; i < game.getPlayers().size(); i++) {
        const auto& player = game.getPlayers()[i];
        printf("    player %zu found...\n", i);
        for (auto& entity : player->getEntities()) {
            printf("    avant if statement...\n");
            if (game.getEntitySelected() && entity == currentPlayer->getEntities()[game.getSelectedEntityIndex()]) {
                continue;
            }
            std::string textureKey = entity->getName();
            printf("    rendering entity %s...\n", textureKey.c_str());
            renderEntity(*entity, game.getTextures().at(iconsMap.at(textureKey)));
            printf("    entity %s rendered...\n", textureKey.c_str());
        }
    }

    if (game.getEntitySelected()) {
        renderSelectedEntity();
    }
}

void RenderGame::renderGrid() const {
    game.getGrid().draw(renderer, game.getCameraX(), game.getCameraY());
}

void RenderGame::renderPlayerInfo() const {
    auto& currentPlayer = game.getPlayers()[game.getPlayerTurn()];
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    RenderData renderData(renderer, font, game.getTextures(), iconsMap);

    std::string coinsnumber = std::to_string(currentPlayer->getCoins());
    SDL_Rect coinRect = {10, 60, 50, 30};
    renderData.renderImageWithText(coinRect, "coin", coinsnumber);

    int totalupkeep = game.getGrid().getNbCasesColor(currentPlayer->getColor());
    for (const auto& entity : currentPlayer->getEntities()) {
        totalupkeep -= entity->getUpkeep();
    }
    std::string stringupkeep = std::to_string(totalupkeep);
    SDL_Rect upkeepRect = {10, 100, 50, 60};
    renderData.renderImageWithText(upkeepRect, totalupkeep < 0 ? "deficit" : "surplus", stringupkeep);

    TTF_CloseFont(font);
}

void RenderGame::renderButtons() const {
    for (const auto& button : game.getUnitButtons()) {
        renderButton(button);
    }
}

void RenderGame::renderSelectedEntity() const {
    const auto& currentPlayer = game.getPlayers()[game.getPlayerTurn()];
    const auto& playerEntities = currentPlayer->getEntities();
    const Entity& selectedEntity = *playerEntities[game.getSelectedEntityIndex()];
    SDL_Rect entityRect = game.entityToRect(selectedEntity);
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    entityRect.x = mouseX - entityRect.w / 2;
    entityRect.y = mouseY - entityRect.h / 2;
    SDL_RenderCopy(renderer, game.getTextures().at(iconsMap.at(selectedEntity.getName())), NULL, &entityRect);
}

void RenderGame::drawButtonCostText(const Button& button, const SDL_Rect& buttonRect) const {
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 16);
    if (font) {
        SDL_Rect textBgRect = {buttonRect.x, buttonRect.y + buttonRect.h - 20, buttonRect.w, 20};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &textBgRect);

        SDL_Color textColor = {255, 255, 0, 255};
        std::string costText = std::to_string(button.getCost());
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, costText.c_str(), textColor);

        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                int textWidth = textSurface->w;
                int textHeight = textSurface->h;
                SDL_Rect costRect = {buttonRect.x + (buttonRect.w - textWidth) / 2, buttonRect.y + buttonRect.h - textHeight - 2, textWidth, textHeight};
                SDL_RenderCopy(renderer, textTexture, NULL, &costRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        TTF_CloseFont(font);
    }
}

void RenderGame::renderAll() const {
    printf("Rendering...\n");
    renderGrid();
    printf("grid render...\n");
    renderPlayers();
    printf("players render...\n");
    renderPlayerInfo();
    printf("player info render...\n");
    renderButtons();
    printf("buttons render...\n");
}