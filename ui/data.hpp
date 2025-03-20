#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

class RenderData {
public:
    // Constructor that matches the arguments with raw pointers
    RenderData(SDL_Renderer* renderer,
               TTF_Font* font,
               const std::vector<SDL_Texture*>& textures,
               const std::map<std::string, int>& iconsMap)
        : renderer(renderer), font(font), textures(textures), iconsMap(iconsMap) {}

    void renderImageWithText(const SDL_Rect& imageRect, const std::string& iconKey, const std::string& text);

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    const std::vector<SDL_Texture*>& textures;
    const std::map<std::string, int>& iconsMap;
};
