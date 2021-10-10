#ifndef COLOR_HH
#define COLOR_HH

#include <cstdint>
#include <iostream>

struct Color {
    Color() : red(0), green(0), blue(0) { }
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) { }
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Read a color as three consecutive bytes (red, green and blue)
inline std::istream& operator>>(std::istream& in, Color& c) {
    if (in) {
        // Using in.get() as >> would discard bytes corresponding to spaces
        c.red = uint8_t(in.get());
        c.green = uint8_t(in.get());
        c.blue = uint8_t(in.get());
    }
    return in;
}

// Write a color as three consecutive bytes (red, green and blue)
inline std::ostream& operator<<(std::ostream& out, const Color& c) {
    return out << c.red << c.green << c.blue;
}

inline bool operator==(Color lhs, Color rhs) {
    return lhs.red == rhs.red
        && lhs.green == rhs.green
        && lhs.blue == rhs.blue;
}

inline bool operator!=(Color lhs, Color rhs) {
    return !(lhs == rhs);
}

#endif
