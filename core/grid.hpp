#ifndef HEXAGONAL_GRID_HPP
#define HEXAGONAL_GRID_HPP

#include <vector>
#include <map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

// matching between the colors like "r" to the color value
extern const std::map<char, SDL_Color> colorMap;

// Point structure for rendering
struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

// Hex class for cube coordinates
class Hex {
private:
    int q, r, s;
public:
    Hex(int q, int r, int s);

    // Basic operations
    Hex add(const Hex& other) const;
    Hex subtract(const Hex& other) const;
    Hex scale(int factor) const;

    // Distance between two hexes
    int distance(const Hex& other) const;

    // Equality operator
    bool operator==(const Hex& other) const;

    // Comparison operator for std::map
    bool operator<(const Hex& other) const {
        if (q != other.q) return q < other.q;
        if (r != other.r) return r < other.r;
        return s < other.s;
    }
    // Getters
    int getQ() const { return q; }
    int getR() const { return r; }
    int getS() const { return s; }

    // Setters
    void setQ(int q) { this->q = q; }
    void setR(int r) { this->r = r; }
    void setS(int s) { this->s = s; }
};


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
    Hex pixelToHex(int x, int y) const;

    // Check if a hex exists in the grid
    bool hexExists(const Hex& hex) const; 

    // Handle mouse click
    void handleMouseClick(int mouseX, int mouseY);

    // Handle mouse motion
    void handleMouseMotion(int mouseX, int mouseY);

    // Draw the grid
    void draw(SDL_Renderer* renderer) const;

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

};

#endif // HEXAGONAL_GRID_HPP
