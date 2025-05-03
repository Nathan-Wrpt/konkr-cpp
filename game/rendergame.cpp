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

void RenderGame::renderButton(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const {
    SDL_Rect buttonRect = button.getRect();

    // Draw button background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);
    
    // Draw button icon
    SDL_Texture* iconTexture = textures[getIconIndex(button.getIconName())];
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);

    // Draw cost text with better visibility
    renderButtonText(renderer, button, buttonRect);
}

void RenderGame::renderButtonNoBorder(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const {
    SDL_Rect buttonRect = button.getRect();

    // Draw button icon
    SDL_Texture* iconTexture = textures[getIconIndex(button.getIconName())];
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);
}

void RenderGame::renderButtonText(SDL_Renderer* renderer, const Button& button, SDL_Rect& buttonRect) const {
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 16); // Larger font size
    if (font) {
        // Create a small background for the text
        SDL_Rect textBgRect = {
            buttonRect.x,
            buttonRect.y + buttonRect.h - 20,
            buttonRect.w,
            20
        };

        // Draw a semi-transparent black background for the text
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &textBgRect);

        SDL_Color textColor = {255, 255, 0, 255};

        std::string costText = std::to_string(button.getCost());
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, costText.c_str(), textColor);

        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            if (textTexture) {
                // Center the text in the background
                int textWidth = textSurface->w;
                int textHeight = textSurface->h;

                SDL_Rect costRect = {
                    buttonRect.x + (buttonRect.w - textWidth) / 2,
                    buttonRect.y + buttonRect.h - textHeight - 2,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, textTexture, NULL, &costRect);
                SDL_DestroyTexture(textTexture);
            }

            SDL_FreeSurface(textSurface);
        }

        TTF_CloseFont(font);
    }
}

void RenderGame::renderTurnButton(SDL_Renderer* renderer, const Button& turnButton, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn) const {
    SDL_Rect buttonRect = turnButton.getRect();
    auto& currentPlayer = players[playerTurn];

    // Determine the correct texture for the turn button
    SDL_Texture* iconTexture = determineTurnButtonTexture(textures, turnButton, currentPlayer);

    // Draw button icon
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);
}

SDL_Texture* RenderGame::determineTurnButtonTexture(const std::vector<SDL_Texture*>& textures, const Button& turnButton, const std::shared_ptr<Player>& currentPlayer) const {
    SDL_Texture* iconTexture = textures[getIconIndex(turnButton.getIconName())];
    bool allEntitiesMoved = std::all_of(currentPlayer->getEntities().begin(), currentPlayer->getEntities().end(), [](const auto& entity) {
        return entity->hasMoved();
    });

    if (allEntitiesMoved && currentPlayer->getCoins() < 10) {
        iconTexture = textures[getIconIndex("nextbright")];
    }
    return iconTexture;
}

template <typename T>
void RenderGame::renderEntities(SDL_Renderer* renderer, const std::vector<std::shared_ptr<T>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const {
    for (const auto& entity : entities) {
        render_entity(renderer, *entity, textures[getIconIndex(textureKey)], grid, cameraX, cameraY);
    }
}

// Explicit template instantiation
template void RenderGame::renderEntities<Bandit>(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Bandit>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;
template void RenderGame::renderEntities<BanditCamp>(SDL_Renderer* renderer, const std::vector<std::shared_ptr<BanditCamp>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;
template void RenderGame::renderEntities<Treasure>(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Treasure>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;
template void RenderGame::renderEntities<Devil>(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Devil>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;
template void RenderGame::renderEntities<Forest>(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Forest>>& entities, const std::string& textureKey, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures) const;

void RenderGame::renderPlayersEntities(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const HexagonalGrid& grid, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures, bool entitySelected, int selectedEntityIndex) const {
    auto& currentPlayer = players[playerTurn];

    for (size_t i = 0; i < players.size(); i++) {
        const auto& player = players[i];
        for (const auto& entity : player->getEntities()) {
            if (entitySelected && entity == currentPlayer->getEntities()[selectedEntityIndex]) {
                continue;
            }
            render_entity(renderer, *entity, textures[getIconIndex(entity->getName())], grid, cameraX, cameraY);
        }
    }
}

void RenderGame::renderSelectedEntity(SDL_Renderer* renderer, size_t playerTurn, const HexagonalGrid& grid, const GameEntities& gameEntities, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures, bool entitySelected, int selectedEntityIndex) const {
    if (entitySelected) {
        const auto& playerEntities = gameEntities.players[playerTurn]->getEntities();
        const std::shared_ptr<Entity>& selectedEntityptr = playerEntities[selectedEntityIndex];
        const Entity& selectedEntity = *playerEntities[selectedEntityIndex];
        SDL_Rect entityRect = entityToRect(selectedEntity, grid, cameraX, cameraY);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        entityRect.x = mouseX - entityRect.w / 2;
        entityRect.y = mouseY - entityRect.h / 2;
        SDL_RenderCopy(renderer, textures[getIconIndex(selectedEntity.getName())], NULL, &entityRect);

        highlightAccessibleHexes(renderer, selectedEntityptr, grid, cameraX, cameraY, playerTurn, gameEntities, textures);
    }
}

void RenderGame::highlightAccessibleHexes(SDL_Renderer* renderer, const std::shared_ptr<Entity>& selectedEntity, const HexagonalGrid& grid, int cameraX, int cameraY, size_t playerTurn, const GameEntities& gameEntities, const std::vector<SDL_Texture*>& textures) const {
    if (selectedEntity->getName() != "castle") {
        for (const auto& hex : grid.getHexes()) {
            bool banditOnHex = std::any_of(gameEntities.bandits.begin(), gameEntities.bandits.end(), [&](const auto& bandit) {
                return bandit->getHex() == hex;
            });
            if (entityManager.HexNotOnTerritoryAndAccessible(selectedEntity, hex, grid, playerTurn, gameEntities) || (grid.getHexColors().at(hex) == gameEntities.players[playerTurn]->getColor() && banditOnHex)) {
                drawHexHighlight(renderer, hex, grid, cameraX, cameraY, gameEntities, textures);
            }
        }
    }
}

void RenderGame::drawHexHighlight(SDL_Renderer* renderer, const Hex& hex, const HexagonalGrid& grid, int cameraX, int cameraY, const GameEntities& gameEntities, const std::vector<SDL_Texture*>& textures) const {
    Point hexPos = grid.hexToPixel(hex);
    SDL_Rect hexRect;
    hexRect.x = static_cast<int>(hexPos.x - grid.getHexSize() / 2) - cameraX;
    hexRect.y = static_cast<int>(hexPos.y - grid.getHexSize() / 2) - cameraY;
    hexRect.w = static_cast<int>(grid.getHexSize());
    hexRect.h = static_cast<int>(grid.getHexSize());

    if (entityManager.entityOnHex(hex, gameEntities)) {
        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 100); // Red color
        SDL_RenderDrawRect(renderer, &hexRect);
        SDL_Rect swordRect = {hexRect.x + hexRect.w / 2 - 10, hexRect.y + hexRect.h / 2 - 10, 20, 20};
        SDL_RenderCopy(renderer, textures[getIconIndex("swords")], NULL, &swordRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 0, 100); // Yellow color
        SDL_RenderDrawRect(renderer, &hexRect);
    }
}

void RenderGame::renderPlayerInfo(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const HexagonalGrid& grid, const std::vector<SDL_Texture*>& textures) const {
    if (playerTurn < players.size()) {
        SDL_Color currentColor = players[playerTurn]->getColor();
        SDL_Rect colorRect = {10, 10, 50, 30};
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        SDL_RenderFillRect(renderer, &colorRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &colorRect);
    }

    renderPlayerResources(renderer, grid, players, playerTurn, textures);
}

void RenderGame::renderPlayerResources(SDL_Renderer* renderer, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, const std::vector<SDL_Texture*>& textures) const {
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    RenderData renderData(renderer, font, textures);

    std::string coinsnumber = std::to_string(players[playerTurn]->getCoins());
    SDL_Rect coinRect = {10, 60, 50, 30};
    if (players[playerTurn]->getCoins() < 50) {
        renderData.renderImageWithText(coinRect, "coin", coinsnumber);
    } else {
        renderData.renderImageWithText(coinRect, "coins", coinsnumber);
    }

    int totalupkeep = grid.getNbCasesColor(players[playerTurn]->getColor());
    for (const auto& entity : players[playerTurn]->getEntities()) {
        totalupkeep -= entity->getUpkeep();
    }
    std::string stringupkeep = std::to_string(totalupkeep);
    SDL_Rect upkeepRect = {10, 100, 50, 60};
    if (totalupkeep < 0) {
        renderData.renderImageWithText(upkeepRect, "deficit", stringupkeep);
    } else {
        renderData.renderImageWithText(upkeepRect, "surplus", stringupkeep);
    }

    TTF_CloseFont(font);
}

void RenderGame::renderAllButtons(SDL_Renderer* renderer, const std::vector<Button>& unitButtons, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, const int&nbplayers, size_t playerTurn, const Button& turnButton, const Button& undoButton, const Button& quitButton, const Button& replayButton) const {
    for (const auto& button : unitButtons) {
        renderButton(renderer, button, textures);
    }

    // Render the undo and quit buttons
    renderButtonNoBorder(renderer, undoButton, textures);
    renderButtonNoBorder(renderer, quitButton, textures);

    // Render the replay button
    if (nbplayers == 1) {
        renderButtonNoBorder(renderer, replayButton, textures);
    } else {
        // Render the turn button
        renderTurnButton(renderer, turnButton, textures, players, playerTurn);
    }
}

void RenderGame::RenderButtonInfo(SDL_Renderer* renderer, Button button, const std::vector<SDL_Texture*>& textures) const {
    SDL_Rect buttonRect = button.getRect();
    int buttonSpacing = 10;
    int ratioInfoW = 27;
    int ratioInfoH = 4;
    int multiplier = 8;

    int infoW = ratioInfoW * multiplier;
    int infoX = buttonRect.x - infoW / 2 + buttonRect.w / 2;
    int infoH = ratioInfoH * multiplier;
    int infoY = buttonRect.y - infoH - buttonSpacing;
    auto infoRect = SDL_Rect{infoX, infoY, infoW, infoH};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &infoRect);
    std::string iconName = button.getIconName();
    if (iconName == "villager") {
        iconName = "info-villager";
    } else if (iconName == "pikeman") {
        iconName = "info-pikeman";
    } else if (iconName == "knight") {
        iconName = "info-knight";
    } else if (iconName == "hero") {
        iconName = "info-hero";
    } else if (iconName == "castle") {
        iconName = "info-castle";
    }
    SDL_RenderCopy(renderer, textures[getIconIndex(iconName)], NULL, &infoRect);
}

void RenderGame::renderGameOverMessage(SDL_Renderer* renderer, const std::vector<std::shared_ptr<Player>>& players, const std::vector<Button>& unitButtons) const {
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    int buttonY = windowHeight - unitButtons[0].getRect().h - 20 - 10;
    int messageY = buttonY - 80;

    SDL_Rect messageBgRect = {windowWidth / 2 - 190, messageY, 390, 60};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &messageBgRect);
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &messageBgRect);

    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255};
        std::string gameOverText = "Game over! Player";

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {messageBgRect.x + 20, messageBgRect.y + (messageBgRect.h - textSurface->h) / 2, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

                SDL_Rect winnerColorRect = {textRect.x + textRect.w + 10, textRect.y + (textRect.h - 30) / 2, 30, 30};
                SDL_Color winnerColor;
                for (auto& player : players) {
                    if (player->isAlive()) {
                        winnerColor = player->getColor();
                        break;
                    }
                }
                SDL_SetRenderDrawColor(renderer, winnerColor.r, winnerColor.g, winnerColor.b, winnerColor.a);
                SDL_RenderFillRect(renderer, &winnerColorRect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &winnerColorRect);

                std::string hasWonText = "has won!";
                SDL_Surface* wonSurface = TTF_RenderText_Solid(font, hasWonText.c_str(), textColor);
                if (wonSurface) {
                    SDL_Texture* wonTexture = SDL_CreateTextureFromSurface(renderer, wonSurface);
                    if (wonTexture) {
                        SDL_Rect wonRect = {winnerColorRect.x + winnerColorRect.w + 10, textRect.y, wonSurface->w, wonSurface->h};
                        SDL_RenderCopy(renderer, wonTexture, NULL, &wonRect);
                        SDL_DestroyTexture(wonTexture);
                    }
                    SDL_FreeSurface(wonSurface);
                }
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        TTF_CloseFont(font);
    }
}