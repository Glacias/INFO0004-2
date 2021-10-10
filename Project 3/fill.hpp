#ifndef FILL_HPP
#define FILL_HPP

#include "color.hh"
#include "shape.hpp"

struct Fill {
  Fill(std::shared_ptr<Shape>& s, std::shared_ptr<Color>& c) : sha(s), col(c) { }
  std::shared_ptr<Shape> sha;
  std::shared_ptr<Color> col;
};

#endif
