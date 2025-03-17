#include "button.hpp"

Button::Button(int x, int y, int width, int height, const std::string& iconName, int cost)
    : rect{x, y, width, height}, iconName(iconName), cost(cost) {}

bool Button::containsPoint(int x, int y) const {
    return x >= rect.x && x < rect.x + rect.w &&
           y >= rect.y && y < rect.y + rect.h;
}
