#include "grid.hpp"
#include <stdexcept>
#include <cmath>
#include <limits>

// --- Hex Class Implementation ---

Hex::Hex(int q, int r, int s) : q(q), r(r), s(s) {
    if (q + r + s != 0) {
        throw std::invalid_argument("Invalid cube coordinates: q + r + s must != 0");
    }
}

Hex Hex::add(const Hex& other) const {
    return Hex(q + other.q, r + other.r, s + other.s);
}

Hex Hex::subtract(const Hex& other) const {
    return Hex(q - other.q, r - other.r, s - other.s);
}

Hex Hex::scale(int factor) const {
    return Hex(q * factor, r * factor, s * factor);
}

int Hex::distance(const Hex& other) const {
    return (std::abs(q - other.q) + std::abs(r - other.r) + std::abs(s - other.s)) / 2;
}

bool Hex::operator==(const Hex& other) const {
    return q == other.q && r == other.r && s == other.s;
}

// --- HexagonalGrid Class Implementation ---

HexagonalGrid::HexagonalGrid(double hexSize) : hexSize(hexSize), offsetX(0), offsetY(0), hoveredHex(nullptr) {}

void HexagonalGrid::generateFromASCII(const std::vector<std::string>& asciiMap, int windowWidth, int windowHeight) {
    hexes.clear();
    hexColors.clear();

    // Parse the ASCII map and generate hexes
    for (size_t row = 0; row < asciiMap.size(); ++row) {
        const std::string& line = asciiMap[row];
        for (size_t col = 0; col < line.size(); ++col) {
            char c = line[col];
            if (c == '.' || c == 'G') { // '.' represents a default hex, 'G' represents a green hex
                // Convert ASCII coordinates to axial coordinates
                int q = col - (row / 2); // Adjust for odd-r offset
                int r = row;
                int s = -q - r;
                Hex hex(q, r, s);
                hexes.push_back(hex);

                // Assign color based on character
                if (c == 'G') {
                    hexColors[hex] = {0, 255, 0, SDL_ALPHA_OPAQUE}; // Green color
                } else {
                    hexColors[hex] = {255, 255, 255, SDL_ALPHA_OPAQUE}; // Default color: white
                }
            }
        }
    }

    // Calculate the bounding box of the grid in pixel space
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& hex : hexes) {
        Point pixel = hexToPixel(hex);
        if (pixel.x < minX) minX = pixel.x;
        if (pixel.x > maxX) maxX = pixel.x;
        if (pixel.y < minY) minY = pixel.y;
        if (pixel.y > maxY) maxY = pixel.y;
    }

    // Calculate the center of the grid
    double gridWidth = maxX - minX;
    double gridHeight = maxY - minY;
    double gridCenterX = minX + gridWidth / 2.0;
    double gridCenterY = minY + gridHeight / 2.0;

    // Calculate the offset to center the grid in the window
    offsetX = (windowWidth / 2.0) - gridCenterX;
    offsetY = (windowHeight / 2.0) - gridCenterY;
}

Point HexagonalGrid::hexToPixel(const Hex& hex) const {
    double x = hexSize * (std::sqrt(3) * hex.getQ() + std::sqrt(3) / 2 * hex.getR()) + offsetX;
    double y = hexSize * (3.0 / 2 * hex.getR()) + offsetY;
    return Point(x, y);
}

Hex HexagonalGrid::pixelToHex(int x, int y) const {
    double fx = x - offsetX;
    double fy = y - offsetY;

    double q = (std::sqrt(3) / 3 * fx - 1.0 / 3 * fy) / hexSize;
    double r = (2.0 / 3 * fy) / hexSize;
    double s = -q - r;

    // Round to the nearest hex
    int rq = std::round(q);
    int rr = std::round(r);
    int rs = std::round(s);

    double q_diff = std::abs(rq - q);
    double r_diff = std::abs(rr - r);
    double s_diff = std::abs(rs - s);

    if (q_diff > r_diff && q_diff > s_diff) {
        rq = -rr - rs;
    } else if (r_diff > s_diff) {
        rr = -rq - rs;
    } else {
        rs = -rq - rr;
    }

    return Hex(rq, rr, rs);
}

bool HexagonalGrid::hexExists(const Hex& hex) const {
    return hexColors.find(hex) != hexColors.end();
}

void HexagonalGrid::handleMouseClick(int mouseX, int mouseY) {
    Hex clickedHex = pixelToHex(mouseX, mouseY);

    // Check if the clicked hex is in the grid
    if (hexColors.find(clickedHex) != hexColors.end()) {
        hexColors[clickedHex] = {255, 0, 0, SDL_ALPHA_OPAQUE}; // Change color to red
    }
}

void HexagonalGrid::handleMouseMotion(int mouseX, int mouseY) {
    Hex hovered = pixelToHex(mouseX, mouseY);

    // Check if the hovered hex is in the grid
    if (hexColors.find(hovered) != hexColors.end()) {
        // If the hovered hex is different from the previous one
        if (!hoveredHex || (*hoveredHex == hovered) == false) {
            // Restore the color of the previously hovered hex
            // if (hoveredHex) {
            //     hexColors[*hoveredHex] = {255, 255, 255, SDL_ALPHA_OPAQUE}; // White
            // }

            // Update the hovered hex and set its color to yellow
            // hoveredHex = &hexColors.find(hovered)->first;
            // hexColors[hovered] = {255, 255, 0, SDL_ALPHA_OPAQUE}; // Yellow
        }
    } else {
        // If no hex is hovered, restore the color of the previously hovered hex
        // if (hoveredHex) {
        //     hexColors[*hoveredHex] = {255, 255, 255, SDL_ALPHA_OPAQUE}; // White
        //     hoveredHex = nullptr;
        // }
    }
}

void HexagonalGrid::draw(SDL_Renderer* renderer) const {
    for (const auto& hex : hexes) {
        Point center = hexToPixel(hex);

        // Get the color of the hex
        SDL_Color color = hexColors.at(hex);

        // Calculate the points of the hexagon
        Sint16 xPoints[6];
        Sint16 yPoints[6];
        for (int i = 0; i < 6; ++i) {
            double angle = 2 * M_PI / 6 * (i + 0.5); // Pointy-top hex
            xPoints[i] = static_cast<Sint16>(center.x + hexSize * std::cos(angle));
            yPoints[i] = static_cast<Sint16>(center.y + hexSize * std::sin(angle));
        }

        // Fill the hexagon with the specified color
        filledPolygonRGBA(renderer, xPoints, yPoints, 6, color.r, color.g, color.b, color.a);

        // Draw the outline of the hexagon
        aapolygonRGBA(renderer, xPoints, yPoints, 6, 0, 0, 0, 255);
    }
}