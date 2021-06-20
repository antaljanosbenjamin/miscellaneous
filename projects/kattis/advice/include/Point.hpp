#pragma once

struct Point {
  double x;
  double y;
};

Point &RotateByDegree(Point &point, const double dAngleInDegree);

double GetLength(const Point &point);

Point operator+(const Point &lhs, const Point &rhs);
Point &operator+=(Point &lhs, const Point &rhs);
Point operator-(const Point &lhs, const Point &rhs);
Point &operator-=(Point &lhs, const Point &rhs);
bool operator==(const Point &lhs, const Point &rhs);
