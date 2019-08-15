#include "Point.hpp"

#include <cmath>

const Point operator+(const Point &lhs, const Point &rhs) {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

const Point &operator+=(Point &lhs, const Point &rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

const bool operator==(const Point &lhs, const Point &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

const Point operator-(const Point &lhs, const Point &rhs) {
  return Point{lhs.x - rhs.x, lhs.y - rhs.y};
}

const Point &operator-=(Point &lhs, const Point &rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

const auto PI = std::acos(-1);

Point &RotateByDegree(Point &point, double dAngleInDegree) {
  double dAngleInRadian = dAngleInDegree / 180 * PI;
  double dNewX = point.x * cos(dAngleInRadian) - point.y * sin(dAngleInRadian);
  double dNewY = point.x * sin(dAngleInRadian) + point.y * cos(dAngleInRadian);

  point.x = dNewX;
  point.y = dNewY;

  return point;
}

const double GetLength(const Point &point) {
  return sqrt(point.x * point.x + point.y * point.y);
}
