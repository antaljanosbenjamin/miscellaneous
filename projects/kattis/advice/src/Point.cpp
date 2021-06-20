#include "Point.hpp"

#include <cmath>

Point operator+(const Point &lhs, const Point &rhs) {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Point &operator+=(Point &lhs, const Point &rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

bool operator==(const Point &lhs, const Point &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

Point operator-(const Point &lhs, const Point &rhs) {
  return Point{lhs.x - rhs.x, lhs.y - rhs.y};
}

Point &operator-=(Point &lhs, const Point &rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

Point &RotateByDegree(Point &point, const double dAngleInDegree) {
  auto angleInRadian = [](const double angle) {
    const auto PI = std::acos(-1);
    const auto arcOfPI = 180;
    return angle / arcOfPI * PI;
  };
  double dAngleInRadian = angleInRadian(dAngleInDegree);
  double dNewX = point.x * cos(dAngleInRadian) - point.y * sin(dAngleInRadian);
  double dNewY = point.x * sin(dAngleInRadian) + point.y * cos(dAngleInRadian);

  point.x = dNewX;
  point.y = dNewY;

  return point;
}

double GetLength(const Point &point) {
  return sqrt(point.x * point.x + point.y * point.y);
}
