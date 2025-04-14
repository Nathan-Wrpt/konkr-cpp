#ifndef HEX_HPP
#define HEX_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

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

#endif // HEX_HPP