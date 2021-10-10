#include "point.hpp"

Point::Point(double x, double y)
{
  _x = x;
  _y = y;
}

//Return the point corresponding to a rotation
// around point 'c' of 'theta' degrees
Point Point::rot(double theta, Point c) const
{
  //Degrees to radiant
  theta = theta*PI/180.0;
  //Relative coordinate
  double px = (_x-c._x);
  double py = (_y-c._y);
  double cosTheta = cos(theta);
  double sinTheta = sin(theta);
  //Rotate the point
  Point p = Point(px*cosTheta-py*sinTheta+c._x, py*cosTheta+px*sinTheta+c._y);
  return p;
}
