#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <string>
#include <vector>
#include <utility>
#include <limits>

#include "point.hpp"

class Shape {
public:
  Shape() :_max_x(0),_max_y(0),_min_x(0),_min_y(0) { }

  /*
  * inside returns : -true if the point p is inside the shape
  *                  -false if not
  * Arguments :
  *  - p : a point
  */
  virtual bool inside(Point p) const = 0;

  /*
  * namedPoint returns corresponding the named point of the shape
  *
  * Arguments :
  *  - dir : a string indicating wich named point is required
  */
  virtual Point namedPoint(std::string dir)const = 0;

  double max_x() const { return _max_x; }
  double max_y() const { return _max_y; }
  double min_x() const { return _min_x; }
  double min_y() const { return _min_y; }

protected:
  //Max and min value that x and y could be while inside the circle
  //Used to limit the search of points/pixels inside the shape
  double _max_x;
  double _max_y;
  double _min_x;
  double _min_y;
};

#endif
