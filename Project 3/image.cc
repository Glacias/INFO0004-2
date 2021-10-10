#include "image.hh"

#include <iostream>
#include <string>

using namespace std;

istream& operator>>(istream& in, Image& img)
{
    // Read header
    string code;
    size_t width = 0, height = 0, max = 0;
    in >> code >> width >> height >> max;
    int space = in.get();
    if (!in.good() || code != "P6" || width == 0 || height == 0 || max != 255
            || !isspace(space)) {
        in.setstate(ios_base::failbit);
        return in;
    }

    // Read pixels
    auto pixels = make_unique<Color[]>(height * width);
    for (size_t y = 0; y < height; ++y)
        for (size_t x = 0; x < width; ++x)
            in >> pixels[(height - y - 1) * width + x]; // Reverse Y direction
    if (!in.good()) {
        in.setstate(ios_base::failbit);
        return in;
    }

    // Copy/move read values to image
    img._width = width;
    img._height = height;
    img._pixels = move(pixels);

    return in;
}

ostream& operator<<(ostream& out, const Image& img)
{
    assert(img.height() > 0);
    assert(img.width() > 0);

    // Write header
    out << "P6 " << img.width() << " " << img.height() << " 255\n";

    // Write pixels
    for (size_t y = 0; y < img.height(); ++y)
        for (size_t x = 0; x < img.width(); ++x)
            out << img(x, img.height() - y - 1); // Reverse Y direction

    return out;
}

void Image::setPixel(size_t x, size_t y, shared_ptr<Color> c)
{
  assert(x < _width);
  assert(y < _height);
  //If a pixel is set, never change it again
  //if(!_pixelDone[y * _width + x])
  //{
    _pixels[y * _width + x] = (*c);
    //_pixelDone[y * _width + x] = true;
  //}
}
