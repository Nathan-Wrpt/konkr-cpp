#ifndef DATA_HPP
#define DATA_HPP

#include <SDL2/SDL_ttf.h>

#include "../constants/constants.hpp"

class RenderData {
public:
    RenderData(SDL_Renderer* renderer,
               TTF_Font* font,
               const std::vector<SDL_Texture*>& textures)
        : renderer(renderer), font(font), textures(textures) {}

    void renderImageWithText(const SDL_Rect& imageRect, const std::string& iconKey, const std::string& text);

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    const std::vector<SDL_Texture*>& textures;
};

#endif // DATA_HPP