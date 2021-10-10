#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "shape.hpp"

class Rectangle: public Shape {
public:
  Rectangle () { }
  Rectangle(Point c, double w, double h);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  Point center;
  double width;
  double height;
  double midWidth;
  double midHeight;
};

#endif
