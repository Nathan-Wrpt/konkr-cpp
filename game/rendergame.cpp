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
    SDL_Texture* iconTexture = textures[iconsMap.at(button.getIconName())];
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);

    // Draw cost text with better visibility
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

        // Use bright text color for better contrast
        SDL_Color textColor = {255, 255, 0, 255}; // Bright yellow for visibility

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
    
    // if all the ntities of the player have moved, change the color of the button
    bool allEntitiesMoved = true;
    for(const auto& entity : currentPlayer->getEntities()) {
        if(!entity->hasMoved()) {
            allEntitiesMoved = false;
            break;
        }
    }
    SDL_Texture* iconTexture = textures[iconsMap.at(turnButton.getIconName())];
    if(allEntitiesMoved && currentPlayer->getCoins() < 10) {
        iconTexture = textures[iconsMap.at("zznext2")];
    } 
    if(players.size() == 1) {
        iconTexture = textures[iconsMap.at("zzquit")];
    }
    // Draw button icon
    SDL_RenderCopy(renderer, iconTexture, NULL, &buttonRect);
}