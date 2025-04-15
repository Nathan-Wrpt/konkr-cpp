#include "constants.hpp"
#include <array>

const std::array<std::string, 26> iconNames = {
    "bandit",         "bandit-camp",    "castle",      "coin",
    "coins",          "deficit",        "emoji-happy", "face",
    "gold-trophy",    "grave",          "hero",        "knight",
    "pikeman",        "silver-trophy",  "surplus",     "town",
    "treasury",       "upkeep",         "villager",    "swords",
    "next",         "nextbright",        "quit",      "devil",
    "forest",       "undo"
};

int getIconIndex(const std::string& name) {
    for (size_t i = 0; i < iconNames.size(); ++i) {
        if (name == iconNames[i]) {
            return static_cast<int>(i);
        }
    }
    return -1; // Not found
}


const std::vector<Hex> directions = {
    Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
    Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
};

const std::map<char, SDL_Color> colorMap = {
    {'r', {255, 0, 0, SDL_ALPHA_OPAQUE}},   // Red
    {'g', {0, 255, 0, SDL_ALPHA_OPAQUE}},   // Green
    {'b', {0, 0, 255, SDL_ALPHA_OPAQUE}},   // Blue
    {'y', {255, 255, 0, SDL_ALPHA_OPAQUE}}, // Yellow
    {'.', defaultColor},                 // Default color (soil)
    {'k', {0, 0, 0, SDL_ALPHA_OPAQUE}},     // Black
    {'c', {0, 255, 255, SDL_ALPHA_OPAQUE}}, // Cyan
    {'m', {255, 0, 255, SDL_ALPHA_OPAQUE}}, // Magenta
    {'o', {255, 165, 0, SDL_ALPHA_OPAQUE}}, // Orange
    {'p', {255, 192, 203, SDL_ALPHA_OPAQUE}}, // Pink
    {'l', {123, 104, 238, SDL_ALPHA_OPAQUE}}, // Lavender
    {'n', {0, 128, 128, SDL_ALPHA_OPAQUE}}, // Teal
    {'t', {0, 206, 209, SDL_ALPHA_OPAQUE}}, // Turquoise
    {'a', {0, 0, 0, SDL_ALPHA_TRANSPARENT}}  // Transparent
};