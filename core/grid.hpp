#ifndef HEXAGONAL_GRID_HPP
#define HEXAGONAL_GRID_HPP

#include <vector>
#include <map>
#include <string>
#include <SDL2/SDL.h>

// Point structure for rendering
struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

// Hex class for cube coordinates
class Hex {
private:
public:
    int q, r, s;
    const std::string& textureKey;

    Hex(int q, int r, int s, const std::string& textureKey = "");

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
};


// HexagonalGrid class
class HexagonalGrid {
    private:
        std::vector<Hex> hexes;
        std::map<Hex, SDL_Color> hexColors; // Map to store the color of each hex
        std::map<std::string, SDL_Texture*> textureMap; // Map to store textures for each texture key
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
    
        // Handle mouse click
        void handleMouseClick(int mouseX, int mouseY);
    
        // Handle mouse motion
        void handleMouseMotion(int mouseX, int mouseY);
    
        // Load a texture and associate it with a key
        bool loadTexture(const std::string& key, SDL_Renderer* renderer, const std::string& imagePath);
    
        // Draw the grid
        void draw(SDL_Renderer* renderer) const;
    
        // Destructor to clean up textures
        ~HexagonalGrid();
    };

#endif // HEXAGONAL_GRID_HPP
