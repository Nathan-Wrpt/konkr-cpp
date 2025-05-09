#include "data.hpp"

void RenderData::renderImageWithText(const SDL_Rect& imageRect, const std::string& iconKey, const std::string& text) {
    // Check if the iconKey exists in iconNames
    if (getIconIndex(iconKey) == -1) {
        std::cerr << "Error: Icon key not found in iconNames" << std::endl;
        return;
    }

    // Get the texture index from the iconNames
    int textureIndex = getIconIndex(iconKey);

    // Render the image
    SDL_RenderCopy(renderer, textures[textureIndex], NULL, &imageRect);

    // Create the text surface and texture
    SDL_Color textColor = {255, 255, 255, 255};
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> textSurface(
        TTF_RenderText_Solid(font, text.c_str(), textColor),
        &SDL_FreeSurface
    );

    std::shared_ptr<SDL_Texture> textTexture(
        SDL_CreateTextureFromSurface(renderer, textSurface.get()),
        SDL_DestroyTexture
    );

    // Calculate the text rectangle position
    SDL_Rect textRect;
    textRect.x = imageRect.x + imageRect.w + 10;
    textRect.h = 30;
    textRect.y = imageRect.y + (imageRect.h - textRect.h) / 2; // Center the text vertically

    // Calculate the width of the text
    SDL_QueryTexture(textTexture.get(), NULL, NULL, &textRect.w, NULL);

    // Render the text
    SDL_RenderCopy(renderer, textTexture.get(), NULL, &textRect);
}
