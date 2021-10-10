#include "difference.hpp"

using std::string;

Difference::Difference(std::shared_ptr<Shape> s1, std::shared_ptr<Shape> s2)
{
  //Setup the given variables
  sha1 = s1;
  sha2 = s2;

  //Setup the max value that x and y could take while inside the shape
  _max_x = sha1->max_x();
  _max_y = sha1->max_y();

  //Setup the min value that x and y could take while inside the shape
  _min_x = sha1->min_x();
  _min_y = sha1->min_y();
}

//Get named point of difference (shape 1)
Point Difference::namedPoint(string dir) const
{
  return sha1->namedPoint(dir);
}

bool Difference::inside(Point p) const
{
  if(sha1->inside(p) && !(sha2->inside(p)))
  {
    return true;
  }
  return false;
}
