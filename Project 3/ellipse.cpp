#include "ellipse.hpp"

using std::string;

Ellipse::Ellipse(Point c, double ra, double rb)
{
  //Setup the given variables
  center = c;
  radiusA = ra;
  radiusB = rb;
  radiusASquare = pow(ra,2);
  radiusBSquare = pow(rb,2);
  radiusABSquare = radiusBSquare*radiusASquare;

  //Setup the max value that x and y could take while inside the shape
  _max_x = center.x() + radiusA;
  _max_y = center.y() + radiusB;

  //Setup the min value that x and y could take while inside the shape
  _min_x = center.x() - radiusA;
  _min_y = center.y() - radiusB;
}

//Get named point of ellipse
Point Ellipse::namedPoint(string dir) const
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
    return Point(center.x()+radiusA*cos(PI/4), center.y()+radiusB*sin(PI/4));
  }
  //North-West
  else if(dir=="nw")
  {
    return Point(center.x()+radiusA*cos(3*PI/4), center.y()+radiusB*sin(3*PI/4));
  }
  //South-West
  else if(dir=="sw")
  {
    return Point(center.x()+radiusA*cos(5*PI/4), center.y()+radiusB*sin(5*PI/4));
  }
  //South-East
  else if(dir=="se")
  {
    return Point(center.x()+radiusA*cos(7*PI/4), center.y()+radiusB*sin(7*PI/4));
  }
  //focii 1 (right)
  else if(dir=="f1")
  {
    return Point(center.x()+sqrt(abs(radiusASquare - radiusBSquare)), center.y());
  }
  //focii 2 (left)
  else if(dir=="f2")
  {
    return Point(center.x()-sqrt(abs(radiusASquare - radiusBSquare)), center.y());
  }
  else
  {
    double inf = std::numeric_limits<double>::infinity();
    return Point(inf, inf);
  }
}

bool Ellipse::inside(Point p) const
{
  //Distance in y and x between p and c
  double dx = abs(p.x()-center.x());
  double dy = abs(p.y()-center.y());

  return (pow(dx,2)*radiusBSquare + pow(dy,2)*radiusASquare) <= radiusABSquare;
}
