#include "rotation.hpp"

using std::string;

Rotation::Rotation(double d, Point c, std::shared_ptr<Shape> s)
{
  //Setup the given variables
  degree = d;
  double theta = degree*PI/180.0;
  center = c;
  sha = s;
  cosTheta = cos(theta);
  sinTheta = sin(theta);
  cosThetaRev = cos(-theta);
  sinThetaRev = sin(-theta);

  //Creation of the 4 rotated (max and min) corners
  Point rot_min_min = fastRot(sha->min_x(),sha->min_y());
  Point rot_min_max = fastRot(sha->min_x(),sha->max_y());
  Point rot_max_min = fastRot(sha->max_x(),sha->min_y());
  Point rot_max_max = fastRot(sha->max_x(),sha->max_y());

  //Setup the max value that x and y could take while inside the shape
  _max_x = std::max(std::max(rot_min_min.x(), rot_min_max.x()) ,std::max(rot_max_min.x(), rot_max_max.x()));
  _max_y = std::max(std::max(rot_min_min.y(), rot_min_max.y()) ,std::max(rot_max_min.y(), rot_max_max.y()));

  //Setup the min value that x and y could take while inside the shape
  _min_x = std::min(std::min(rot_min_min.x(), rot_min_max.x()) ,std::min(rot_max_min.x(), rot_max_max.x()));
  _min_y = std::min(std::min(rot_min_min.y(), rot_min_max.y()) ,std::min(rot_max_min.y(), rot_max_max.y()));
}

//Get named point of rotation
Point Rotation::namedPoint(string dir) const
{
  Point p = sha->namedPoint(dir);
  return fastRot(p.x(), p.y());
}

bool Rotation::inside(Point p) const
{
  if(sha->inside(fastRotReverse(p.x(),p.y())))
  {
    return true;
  }
  return false;
}

Point Rotation::fastRot(double x, double y) const
{
  double px = (x-center.x());
  double py = (y-center.y());
  return(Point(px*cosTheta-py*sinTheta+center.x(), py*cosTheta+px*sinTheta+center.y()));
}

Point Rotation::fastRotReverse(double x, double y) const
{
  double px = (x-center.x());
  double py = (y-center.y());
  return(Point(px*cosThetaRev-py*sinThetaRev+center.x(), py*cosThetaRev+px*sinThetaRev+center.y()));
}
