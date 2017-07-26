#include "Point.h"

#undef __STRICT_ANSI__

#include <cmath>

const Point Point::operator+( const Point &rhs ) const
{
    return Point{ dX + rhs.dX, dY + rhs.dY };
}

const Point &Point::operator+=( const Point &rhs )
{
    dX += rhs.dX;
    dY += rhs.dY;
    return *this;
}

const bool Point::operator==( const Point &rhs ) const
{
    return dX == rhs.dX && dY == rhs.dY;
}

const Point Point::operator-( const Point &rhs ) const
{
    return Point{ dX - rhs.dX, dY - rhs.dY };
}

const Point &Point::operator-=( const Point &rhs )
{
    dX -= rhs.dX;
    dY -= rhs.dY;
    return *this;
}

Point &Point::rotateByDegree( double dAngleInDegree )
{
    double dAngleInRadian = dAngleInDegree / 180 * M_PI;
    double dNewX = dX * cos( dAngleInRadian ) - dY * sin( dAngleInRadian );
    double dNewY = dX * sin( dAngleInRadian ) + dY * cos( dAngleInRadian );

    dX = dNewX;
    dY = dNewY;

    return *this;
}

const double Point::getLength() const
{
    return sqrt( dX * dX + dY * dY );
}
