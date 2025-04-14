#include "constants.hpp"

std::map<std::string, int> iconsMap = {
    {"bandit", 0},        {"bandit-camp", 1},
    {"castle", 2},        {"coin", 3},
    {"coins", 4},         {"deficit", 5},
    {"emoji-happy", 6},   {"face", 7},
    {"gold-trophy", 8},   {"grave", 9},
    {"hero", 10},         {"knight", 11},
    {"pikeman", 12},      {"silver-trophy", 13},
    {"surplus", 14},      {"town", 15},
    {"treasury", 16},     {"upkeep", 17},
    {"villager", 18},     {"zwords", 19},
    {"zznext", 20},       {"zznext2", 21},
    {"zzquit", 22},       {"zzzdevil", 23}
};

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