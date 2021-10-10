#include "union.hpp"

using std::string;

Union::Union(std::vector<std::shared_ptr<Shape>> v)
{
  //v null or empty
  if(v.size()==0)
  {
    std::cerr << "Union was initialized with a null or empty vector of shape" << std::endl;
    exit(EXIT_FAILURE);
  }

  //Setup the given variables
  shapeVect = v;
  sha = shapeVect.at(0);

  //Setup the max value that x and y could take while inside the shape
  _max_x = sha->max_x();
  _max_y = sha->max_y();

  //Setup the min value that x and y could take while inside the shape
  _min_x = sha->min_x();
  _min_y = sha->min_y();

  //Iterate over all shapes in union to get the max and min of all
  for(size_t i=1; i<shapeVect.size(); i++)
  {
    if(shapeVect[i]->max_x()>_max_x)
    {
      _max_x = shapeVect[i]->max_x();
    }
    if(shapeVect[i]->max_y()>_max_y)
    {
      _max_y = shapeVect[i]->max_y();
    }
    if(shapeVect[i]->min_x()<_min_x)
    {
      _min_x = shapeVect[i]->min_x();
    }
    if(shapeVect[i]->min_y()<_min_y)
    {
      _min_y = shapeVect[i]->min_y();
    }
  }
}

//Get named point of union (first shape)
Point Union::namedPoint(string dir) const
{
  return sha->namedPoint(dir);
}

bool Union::inside(Point p) const
{
  for(std::shared_ptr<Shape> shape : shapeVect)
  {
    if(shape->inside(p))
    {
      return true;
    }
  }
  return false;
}
