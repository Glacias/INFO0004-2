#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <algorithm>
#include <math.h>
#define PI 3.14159265
#include "shape.hpp"

class Rotation: public Shape {
public:
  Rotation() { }
  Rotation(double d, Point c, std::shared_ptr<Shape> s);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  Point fastRot(double x, double y) const;
  Point fastRotReverse(double x, double y) const;
  std::shared_ptr<Shape> sha;
  Point center;
  double degree;
  double cosTheta;
  double sinTheta;
  double cosThetaRev;
  double sinThetaRev;
};

#endif
