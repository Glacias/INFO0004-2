#ifndef POINT_HPP
#define POINT_HPP

#include <math.h>
#include <memory>
#define PI 3.14159265

class Point {
public:
  Point() { }
  Point(double x, double y);
  Point rot(double theta, Point c) const;
  double x() const { return _x; }
  double y() const { return _y; }

protected:
  double _x;
  double _y;
};

#endif
