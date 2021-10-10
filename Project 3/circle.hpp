#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <math.h>
#include "shape.hpp"

class Circle: public Shape {
public:
  Circle() { }
  Circle(Point c, double r);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  Point center;
  double radius;
  double radiusSquare;
};

#endif
