#ifndef IMAGE_HH
#define IMAGE_HH

#include <cassert>
#include <cstddef>
#include <memory>

#include "color.hh"

class Image {
public:
    Image() : _height(0), _width(0) { }

    Image(size_t width, size_t height) : _height(height), _width(width) {
        _pixels = std::make_unique<Color[]>(width * height);
        _pixelDone = std::make_unique<bool[]>(width * height);
    }

    size_t height() const { return _height; }

    size_t width() const { return _width; }

    void setPixel(size_t x, size_t y, std::shared_ptr<Color> c);

    // `img(x,y)` returns the pixel on row `x`, column `y`
    Color& operator() (size_t x, size_t y) {
        assert(x < _width);
        assert(y < _height);
        return _pixels[y * _width + x];
    }

    Color operator() (size_t x, size_t y) const {
        assert(x < _width);
        assert(y < _height);
        return _pixels[y * _width + x];
    }

    // Read image in binary 8-bit PPM format
    friend std::istream& operator>>(std::istream&, Image&);

private:
    std::unique_ptr<Color[]> _pixels;
    std::unique_ptr<bool[]> _pixelDone;
    size_t _height;
    size_t _width;
};

// Write image in binary 8-bit PPM format
std::ostream& operator<<(std::ostream&, const Image&);

#endif
