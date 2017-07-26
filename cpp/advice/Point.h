#ifndef POINT_H
#define POINT_H

struct Point
{
    double dX, dY;

    const Point operator+( const Point &rhs ) const;
    const Point& operator+=( const Point &rhs );
    const Point operator-( const Point &rhs ) const;
    const Point& operator-=( const Point &rhs );
    const bool operator==( const Point &rhs ) const;

    Point& rotateByDegree( double dAngleInDegree );

    const double getLength() const;
};

#endif //POINT_H
