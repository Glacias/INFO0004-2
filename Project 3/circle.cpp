#include "circle.hpp"

Circle::Circle(Point c, double r)
{
  //Setup the given variables
  center = c;
  radius = r;
  radiusSquare = pow(r,2);

  //Setup the max value that x and y could take while inside the shape
  _max_x = center.x() + radius;
  _max_y = center.y() + radius;

  //Setup the min value that x and y could take while inside the shape
  _min_x = center.x() - radius;
  _min_y = center.y() - radius;
}

//Get named point of circle
Point Circle::namedPoint(std::string dir) const
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
    return Point(_max_x,center.y()).rot(45,center);
  }
  //North-West
  else if(dir=="nw")
  {
    return Point(_max_x,center.y()).rot(135,center);
  }
  //South-West
  else if(dir=="sw")
  {
    return Point(_max_x,center.y()).rot(225,center);
  }
  //South-East
  else if(dir=="se")
  {
    return Point(_max_x,center.y()).rot(315,center);
  }
  else
  {
    double inf = std::numeric_limits<double>::infinity();
    return Point(inf, inf);
  }
}

bool Circle::inside(Point p) const
{
  return pow(p.y()-center.y(),2) + pow(p.x()-center.x(),2) <= radiusSquare;
}
