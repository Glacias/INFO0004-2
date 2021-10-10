#include "shift.hpp"

using std::string;

Shift::Shift(Point t, std::shared_ptr<Shape> s)
{
  //Setup the given variables
  vector = t;
  sha = s;

  //Setup the max value that x and y could take while inside the shape
  _max_x = sha->max_x() + t.x();
  _max_y = sha->max_y() + t.y();

  //Setup the min value that x and y could take while inside the shape
  _min_x = sha->min_x() + t.x();
  _min_y = sha->min_y() + t.y();
}

//Get named point of shift
Point Shift::namedPoint(string dir) const
{
  Point oldNamedPoint = sha->namedPoint(dir);
  return Point(oldNamedPoint.x() + vector.x(), oldNamedPoint.y()+ vector.y());
}

bool Shift::inside(Point p) const
{
  if(sha->inside(Point(p.x()-vector.x(),p.y()-vector.y())))
  {
    return true;
  }
  return false;
}
