#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SDL2/SDL.h>
#include <string>
#include <functional>

class Button {
private:
    SDL_Rect rect;
    std::string iconName;
    int cost;

public:
    Button(int x, int y, int width, int height, const std::string& iconName, int cost);
    
    bool containsPoint(int x, int y) const;
    
    SDL_Rect getRect() const { return rect; }
    std::string getIconName() const { return iconName; }
    int getCost() const { return cost; }
};

#endif // BUTTON_HPP
