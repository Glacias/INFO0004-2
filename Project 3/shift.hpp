#ifndef SHIFT_HPP
#define SHIFT_HPP

#include "shape.hpp"
#include <memory>

class Shift: public Shape {
public:
  Shift() { }
  Shift(Point t, std::shared_ptr<Shape> s);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  std::shared_ptr<Shape> sha;
  Point vector;
};

#endif
