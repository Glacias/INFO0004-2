#ifndef DIFFERENCE_HPP
#define DIFFERENCE_HPP

#include "shape.hpp"
#include <memory>

class Difference: public Shape {
public:
  Difference() { }
  Difference(std::shared_ptr<Shape> s1, std::shared_ptr<Shape> s2);
  virtual bool inside(Point p) const;
  virtual Point namedPoint(std::string dir) const;
private:
  std::shared_ptr<Shape> sha1;
  std::shared_ptr<Shape> sha2;
};

#endif
