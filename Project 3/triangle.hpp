#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <math.h>
#include <algorithm>
#include "shape.hpp"

class Triangle: public Shape {
public:
  Triangle() { }
  Triangle(Point a, Point b, Point c);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  Point v0;
  Point v1;
  Point v2;
};

#endif
