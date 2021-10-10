#include "rectangle.hpp"

using std::string;

Rectangle::Rectangle(Point c, double w, double h)
{
  //Setup the given variables
  center = c;
  width = w;
  height = h;
  midWidth = w/2;
  midHeight = h/2;

  //Setup the max value that x and y could take while inside the shape
  _max_x = center.x() + (midWidth);
  _max_y = center.y() + (midHeight);

  //Setup the min value that x and y could take while inside the shape
  _min_x = center.x() - (midWidth);
  _min_y = center.y() - (midHeight);
}

//Get named point of rectangle
Point Rectangle::namedPoint(string dir) const
{
  //Center
  if(dir=="c")
  {
    return center;
  }
  //North
  else if(dir=="n")
  {
    return Point(center.x(),_max_y);
  }
  //South
  else if(dir=="s")
  {
    return Point(center.x(),_min_y);
  }
  //West
  else if(dir=="w")
  {
    return Point(_min_x,center.y());
  }
  //East
  else if(dir=="e")
  {
    return Point(_max_x,center.y());
  }
  //North-East
  else if(dir=="ne")
  {
    return Point(_max_x,_max_y);
  }
  //North-West
  else if(dir=="nw")
  {
    return Point(_min_x,_max_y);
  }
  //South-West
  else if(dir=="sw")
  {
    return Point(_min_x,_min_y);
  }
  //South-East
  else if(dir=="se")
  {
    return Point(_max_x,_min_y);
  }
  else
  {
    double inf = std::numeric_limits<double>::infinity();
    return Point(inf, inf);
  }
}

bool Rectangle::inside(Point p) const
{
  //Distance in y and x between p and c
  double dx = abs(p.x()-center.x());
  double dy = abs(p.y()-center.y());

  return (dx <= midWidth && dy <= midHeight);
}
