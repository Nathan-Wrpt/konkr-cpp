#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <string>
#include <vector>

#include "../core/hex.hpp"

extern std::map<std::string, int> iconsMap;
extern const std::vector<Hex> directions;

const SDL_Color defaultColor = {0, 125, 0, SDL_ALPHA_OPAQUE}; // Dark green
extern const std::map<char, SDL_Color> colorMap;

#endif // CONSTANTS_HPP
