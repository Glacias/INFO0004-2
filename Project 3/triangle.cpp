#include "triangle.hpp"

using std::string;

Triangle::Triangle(Point a, Point b, Point c)
{
  //Setup the given variables
  v0 = a;
  v1 = b;
  v2 = c;

  //Setup the max value that x and y could take while inside the shape
  _max_x = std::max(v0.x(),std::max(v1.x(),v2.x()));
  _max_y = std::max(v0.y(),std::max(v1.y(),v2.y()));

  //Setup the min value that x and y could take while inside the shape
  _min_x = std::min(v0.x(),std::min(v1.x(),v2.x()));
  _min_y = std::min(v0.y(),std::min(v1.y(),v2.y()));
}

//Get named point of triangle
Point Triangle::namedPoint(string dir) const
{
  //v0
  if(dir=="v0")
  {
    return v0;
  }
  //v1
  else if(dir=="v1")
  {
    return v1;
  }
  //v2
  else if(dir=="v2")
  {
    return v2;
  }
  //s01
  else if(dir=="s01")
  {
    return Point((v0.x()+v1.x())/2, (v0.y()+v1.y())/2);
  }
  //s02
  else if(dir=="s02")
  {
    return Point((v0.x()+v2.x())/2, (v0.y()+v2.y())/2);
  }
  //s12
  else if(dir=="s12")
  {
    return Point((v1.x()+v2.x())/2, (v1.y()+v2.y())/2);
  }
  //c
  else if(dir=="c")
  {
    return Point((v0.x()+v1.x()+v2.x())/3, (v0.y()+v1.y()+v2.y())/3);
  }
  else
  {
    double inf = std::numeric_limits<double>::infinity();
    return Point(inf, inf);
  }
}

bool Triangle::inside(Point p) const
{
  //Computing dot product
  double dp1 = (v1.y() - v0.y())*(p.x()- v0.x()) + (-v1.x() + v0.x())*(p.y()- v0.y());
  double dp2 = (v2.y() - v1.y())*(p.x()- v1.x()) + (-v2.x() + v1.x())*(p.y()- v1.y());
  if((dp1>0 && dp2<0) || (dp1<0 && dp2>0))
    return false;
  double dp3 = (v0.y() - v2.y())*(p.x()- v2.x()) + (-v0.x() + v2.x())*(p.y()- v2.y());

  //To be inside they all need to be >0 or all <0
  bool hasNeg = (dp1 < 0) || (dp2 < 0) || (dp3 < 0);
  bool hasPos = (dp1 > 0) || (dp2 > 0) || (dp3 > 0);
  return !(hasNeg && hasPos);
}
