#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <string>
#include <vector>
#include <array>

#include "../core/hex.hpp"

#define NB_ICONS 31

extern const std::array<std::string, NB_ICONS> iconNames;
extern const std::vector<Hex> directions;

const SDL_Color defaultColor = {0, 125, 0, SDL_ALPHA_OPAQUE}; // Dark green
extern const std::map<char, SDL_Color> colorMap;

// Function to get the index of an icon name
int getIconIndex(const std::string& name);

#endif // CONSTANTS_HPP
