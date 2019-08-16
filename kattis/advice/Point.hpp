#pragma once

struct Point {
  double x;
  double y;
};

Point &RotateByDegree(Point &point, double dAngleInDegree);

double GetLength(const Point &point);

const Point operator+(const Point &lhs, const Point &rhs);
const Point &operator+=(Point &lhs, const Point &rhs);
const Point operator-(const Point &lhs, const Point &rhs);
const Point &operator-=(Point &lhs, const Point &rhs);
bool operator==(const Point &lhs, const Point &rhs);
