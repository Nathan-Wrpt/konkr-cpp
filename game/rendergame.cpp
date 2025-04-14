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

void RenderGame::render(SDL_Renderer* renderer, const HexagonalGrid& grid, const std::vector<std::shared_ptr<Bandit>>& bandits, const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, const std::vector<std::shared_ptr<Treasure>>& treasures, const std::vector<std::shared_ptr<Devil>>& devils, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn, int cameraX, int cameraY, const std::vector<SDL_Texture*>& textures, const std::vector<Button>& unitButtons, const Button& turnButton, bool entitySelected, int selectedEntityIndex) const {
    // Draw the grid
    grid.draw(renderer, cameraX, cameraY);

    // Render all bandits
    for (const auto& bandit : bandits) {
        render_entity(renderer, *bandit, textures[iconsMap.at("bandit")], grid, cameraX, cameraY);
    }

    // Render all bandit camps
    for (const auto& banditCamp : banditCamps) {
        render_entity(renderer, *banditCamp, textures[iconsMap.at("bandit-camp")], grid, cameraX, cameraY);
    }

    // Render treasure(s)
    for (const auto& treasure : treasures) {
        render_entity(renderer, *treasure, textures[iconsMap.at("treasury")], grid, cameraX, cameraY);
    }

    // Render devil(s)
    for (const auto& devil : devils) {
        render_entity(renderer, *devil, textures[iconsMap.at("zzzdevil")], grid, cameraX, cameraY);
    }

    auto& currentPlayer = players[playerTurn];

    // Render all players' entities
    for (size_t i = 0; i < players.size(); i++) {
        const auto& player = players[i];
        for (const auto& entity : player->getEntities()) {
            // Skip rendering the selected entity if it belongs to the current player
            if(entitySelected && entity == currentPlayer->getEntities()[selectedEntityIndex]) {
                continue;
            }
            // Determine the texture based on entity type
            std::string textureKey = entity->getName();

            // Render the entity
            render_entity(renderer, *entity, textures[iconsMap.at(textureKey)], grid, cameraX, cameraY);
        }
    }

    // Render of the entity on the cursor if selected
    if(entitySelected) {
        const auto& playerEntities = currentPlayer->getEntities();
        const std::shared_ptr<Entity>& selectedEntityptr = playerEntities[selectedEntityIndex];
        const Entity& selectedEntity = *playerEntities[selectedEntityIndex];
        SDL_Rect entityRect = entityToRect(selectedEntity, grid, cameraX, cameraY);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        entityRect.x = mouseX - entityRect.w / 2;
        entityRect.y = mouseY - entityRect.h / 2;
        SDL_RenderCopy(renderer, textures[iconsMap.at(selectedEntity.getName())], NULL, &entityRect);

        // Draw yellow small rectangles on accessible hexes
        if(selectedEntityptr->getName() != "castle") {
            for (const auto& hex : grid.getHexes()) {
                bool banditOnHex = false;
                for (const auto& bandit : bandits) {
                    if (bandit->getHex() == hex) {
                        banditOnHex = true;
                        break;
                    }
                }
                if (entityManager.HexNotOnTerritoryAndAccessible(selectedEntityptr, hex, grid, players, playerTurn, banditCamps, devils) || (grid.getHexColors().at(hex) == currentPlayer->getColor() && banditOnHex)) {
                    Point hexPos = grid.hexToPixel(hex);
                    SDL_Rect hexRect;
                    hexRect.x = static_cast<int>(hexPos.x - grid.getHexSize() / 2);
                    hexRect.y = static_cast<int>(hexPos.y - grid.getHexSize() / 2);
                    // Adjust the position based on camera as usual
                    hexRect.x -= cameraX;
                    hexRect.y -= cameraY;
                    hexRect.w = static_cast<int>(grid.getHexSize());
                    hexRect.h = static_cast<int>(grid.getHexSize());
                    // Draw the rectangle, red if the hex is occupied and can be attacked
                    if(entityManager.entityOnHex(hex, bandits, banditCamps, treasures, devils, players)) {
                        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 100); // Red color
                        SDL_RenderDrawRect(renderer, &hexRect);
                        // put the texture "zword" on the hex
                        SDL_Rect swordRect = {hexRect.x + hexRect.w / 2 - 10, hexRect.y + hexRect.h / 2 - 10, 20, 20};
                        SDL_RenderCopy(renderer, textures[iconsMap.at("zwords")], NULL, &swordRect);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 150, 150, 0, 100); // Yellow color
                        SDL_RenderDrawRect(renderer, &hexRect);
                    }
                }
            }
        } 
    }

    // Display current player's color at the top of the screen
    if (playerTurn >= 0 && playerTurn < players.size()) {
        SDL_Color currentColor = players[playerTurn]->getColor();

        SDL_Rect colorRect = {10, 10, 50, 30};

        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);

        SDL_RenderFillRect(renderer, &colorRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &colorRect);
    }
    // coinTexture = textures[iconsMap.at("coin")];
    TTF_Font* font = TTF_OpenFont("assets/OpenSans.ttf", 24);
    RenderData renderData(renderer, font, textures, iconsMap);

    // Remplacez le code existant par un appel à renderImageWithText
    std::string coinsnumber = std::to_string(currentPlayer->getCoins());
    SDL_Rect coinRect = {10, 60, 50, 30};
    if (currentPlayer->getCoins() < 50) {
        renderData.renderImageWithText(coinRect, "coin", coinsnumber);
    } else {
        renderData.renderImageWithText(coinRect, "coins", coinsnumber);
    }
    int totalupkeep = grid.getNbCasesColor(currentPlayer->getColor());
    for (const auto& entity : currentPlayer->getEntities()) {
        totalupkeep -= entity->getUpkeep();
    }
    std::string stringupkeep = std::to_string(totalupkeep);
    SDL_Rect upkeepRect = {10, 100, 50, 60};
    if (totalupkeep < 0) {
        renderData.renderImageWithText(upkeepRect, "deficit", stringupkeep);
    } else {
        renderData.renderImageWithText(upkeepRect, "surplus", stringupkeep);
    }

    // Render all buttons
    for (const auto& button : unitButtons) {
        renderButton(renderer, button, textures);
    }

    // Render the turn button
    renderTurnButton(renderer, turnButton, textures, players, playerTurn);
    
    // Display game over message if only one player remains
    if (players.size() == 1) {
        int windowWidth, windowHeight;
        SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
        
        // Position the message above the unit buttons
        int buttonY = windowHeight - unitButtons[0].getRect().h - 20 - 10;
        int messageY = buttonY - 80; // Position above the buttons
        
        // background for the message
        SDL_Rect messageBgRect = {
            windowWidth / 2 - 190,
            messageY,
            390,
            60
        };
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &messageBgRect);
        
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold
        SDL_RenderDrawRect(renderer, &messageBgRect);
        
        // Render "Game over!" text
        if (font) {
            SDL_Color textColor = {255, 255, 255, 255}; // White text
            std::string gameOverText = "Game over! Player";
            
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                
                if (textTexture) {
                    SDL_Rect textRect = {
                        messageBgRect.x + 20,
                        messageBgRect.y + (messageBgRect.h - textSurface->h) / 2,
                        textSurface->w,
                        textSurface->h
                    };
                    
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    
                    // Draw winner's color square
                    SDL_Rect winnerColorRect = {
                        textRect.x + textRect.w + 10,
                        textRect.y + (textRect.h - 30) / 2,
                        30,
                        30
                    };
                    
                    SDL_Color winnerColor = players[0]->getColor();
                    SDL_SetRenderDrawColor(renderer, winnerColor.r, winnerColor.g, winnerColor.b, winnerColor.a);
                    SDL_RenderFillRect(renderer, &winnerColorRect);
                    
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &winnerColorRect);
                    
                    // Render "has won!" text
                    std::string hasWonText = "has won!";
                    SDL_Surface* wonSurface = TTF_RenderText_Solid(font, hasWonText.c_str(), textColor);
                    
                    if (wonSurface) {
                        SDL_Texture* wonTexture = SDL_CreateTextureFromSurface(renderer, wonSurface);
                        
                        if (wonTexture) {
                            SDL_Rect wonRect = {
                                winnerColorRect.x + winnerColorRect.w + 10,
                                textRect.y,
                                wonSurface->w,
                                wonSurface->h
                            };
                            
                            SDL_RenderCopy(renderer, wonTexture, NULL, &wonRect);
                            SDL_DestroyTexture(wonTexture);
                        }
                        
                        SDL_FreeSurface(wonSurface);
                    }
                    
                    SDL_DestroyTexture(textTexture);
                }
                
                SDL_FreeSurface(textSurface);
            }
        }
    }
    
    TTF_CloseFont(font);
}
