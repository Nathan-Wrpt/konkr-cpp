#ifndef HEXAGONAL_GRID_HPP
#define HEXAGONAL_GRID_HPP

#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdexcept>
#include <cmath>
#include <limits>

#include "../constants/constants.hpp"

// Redéfinition de l'opérateur == pour SDL_Color
inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs) {
    return lhs.r == rhs.r &&
           lhs.g == rhs.g &&
           lhs.b == rhs.b &&
           lhs.a == rhs.a;
}

// HexagonalGrid class
class HexagonalGrid {
private:
    std::vector<Hex> hexes;
    std::map<Hex, SDL_Color> hexColors; // Map to store the color of each hex
    double hexSize;
    double offsetX, offsetY; // Offset to center the grid
    const Hex* hoveredHex;

public:
    HexagonalGrid(double hexSize);

    // Generate a grid from an ASCII map
    void generateFromASCII(const std::vector<std::string>& asciiMap, int windowWidth, int windowHeight);

    // Convert hex to pixel
    Point hexToPixel(const Hex& hex) const;

    // Convert pixel to hex
    Hex pixelToHex(int x, int y, int cameraX, int cameraY) const;

    // Check if a hex exists in the grid
    bool hexExists(const Hex& hex) const; 

    // Handle mouse click
    void handleMouseClick(int mouseX, int mouseY, int cameraX, int cameraY);

    // Draw the grid
    void draw(SDL_Renderer* renderer, int cameraX, int cameraY) const;

    // Getter for hexes
    const std::vector<Hex>& getHexes() const { return hexes; }

    // Getter for hex size
    double getHexSize() const { return hexSize; }

    // Getter for hex colors
    const std::map<Hex, SDL_Color>& getHexColors() const { return hexColors; }

    // Set the color of a hex
    void setHexColor(const Hex& hex, const SDL_Color& color);

    // Check if a neighbor of a hex has a specific color
    bool hasNeighborWithColor(const Hex& hex, const SDL_Color& color) const;

    //Get the number of cases in a certain color
    int getNbCasesColor(const SDL_Color& color) const;

    // set HexSize
    void setHexSize(double size) { hexSize = size; }

};

#endif // HEXAGONAL_GRID_HPP
