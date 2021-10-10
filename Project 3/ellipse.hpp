#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <math.h>
#define PI 3.14159265
#include <algorithm>
#include "shape.hpp"

class Ellipse: public Shape {
public:
  Ellipse() { }
  Ellipse(Point c, double ra, double rb);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  Point center;
  double radiusA;
  double radiusB;
  double minRadius;
  double radiusASquare;
  double radiusBSquare;
  double radiusABSquare;
};

#endif
