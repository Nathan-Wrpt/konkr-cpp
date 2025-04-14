#include "hex.hpp"

// --- Hex Class Implementation ---

Hex::Hex(int q, int r, int s) : q(q), r(r), s(s) {
    if (q + r + s != 0) {
        throw std::invalid_argument("Invalid cube coordinates: q + r + s must == 0");
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
