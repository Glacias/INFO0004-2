#ifndef UNION_HPP
#define UNION_HPP

#include "shape.hpp"
#include <iostream>
#include <vector>

class Union: public Shape {
public:
  Union() { }
  Union(std::vector<std::shared_ptr<Shape>> v);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  std::vector<std::shared_ptr<Shape>> shapeVect;
  std::shared_ptr<Shape> sha;
};

#endif
