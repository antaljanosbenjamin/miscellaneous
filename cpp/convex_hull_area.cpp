#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

// Calculates the area of convex hull specified by the points
// Example input:
// 4         | number of points
// 1 2       | first point's coords
// 3 2       | second point's coords
// 4 4       | ...
// 7 20      | nth point's coords
// 0         | 0 or m. If 0, the program terminates, otherwise read m points and calculates again
//
// Output:
// 23

using namespace std;

enum Orientation
{
    COLLINEAR,
    CLOCKWISE,
    COUNTERCLOCKWISE
};

struct Point;
typedef Point Vector;

struct Point
{
    int iX, iY;

    const Point operator+( const Point &rhs ) const;
    const Point operator-( const Point &rhs ) const;
    // Cross-product
    const int operator%( const Point &rhs ) const;
    const bool operator==( const Point &rhs ) const;

    const int getLengthSquare() const;

    static Orientation getOrientation( const Point &pointSource, const Point &pointFirst, const Point &pointSecond );

    static bool compare( const Point &pointSource, const Point &pointLhs, const Point &pointRhs );
};

class ConvexHullCalculator
{
public:
    std::vector<Point> m_vectorPoints;
    Point m_pointMostLeftBottom;
    std::vector<Point> m_vectorConvexHull;

    ConvexHullCalculator( vector<Point> vectorPoints );

    void calculateConvexHull();

    double calculateConvexHullArea();
};

const Point Point::operator+( const Point &rhs ) const
{
    return Point{ iX + rhs.iX, iY + rhs.iY };
}

const bool Point::operator==( const Point &rhs ) const
{
    return iX == rhs.iX && iY == rhs.iY;
}

const Point Point::operator-( const Point &rhs ) const
{
    return Point{ iX - rhs.iX, iY - rhs.iY };
}

// Cross-product
const int Point::operator%( const Point &rhs ) const
{
    return iX * rhs.iY - iY * rhs.iX;
}

const int Point::getLengthSquare() const
{
    return iX * iX + iY * iY;
}

Orientation Point::getOrientation( const Point &pointSource, const Point &pointFirst, const Point &pointSecond )
{
    Vector vecFromSourceToFirst = pointFirst - pointSource;
    Vector vecFromSourceToSecond = pointSecond - pointSource;

    int iCrossProduct = vecFromSourceToFirst % vecFromSourceToSecond;

    if ( iCrossProduct == 0 )
        return COLLINEAR;

    return ( iCrossProduct > 0 ) ? COUNTERCLOCKWISE : CLOCKWISE;
}

bool Point::compare( const Point &pointSource, const Point &pointLhs, const Point &pointRhs )
{
    Orientation orientation = getOrientation( pointSource, pointLhs, pointRhs );

    if ( orientation == 0 )
        return ( pointLhs.getLengthSquare() < pointRhs.getLengthSquare()) ? true : false;

    return ( orientation == COUNTERCLOCKWISE ) ? true : false;
}

ConvexHullCalculator::ConvexHullCalculator( vector<Point> vectorPoints ) : m_vectorPoints( vectorPoints ), m_pointMostLeftBottom(),
                                                                           m_vectorConvexHull()
{
    m_pointMostLeftBottom = m_vectorPoints[0];
    size_t sMostLeftBottomIndex = 0;
    for ( size_t sPointPos = 1; sPointPos < m_vectorPoints.size(); ++sPointPos )
    {
        int iActualY = m_vectorPoints[sPointPos].iY;

        if (( iActualY < m_pointMostLeftBottom.iY ) ||
            ( iActualY == m_pointMostLeftBottom.iY && m_vectorPoints[sPointPos].iX < m_pointMostLeftBottom.iX ))
        {
            m_pointMostLeftBottom = m_vectorPoints[sPointPos];
            sMostLeftBottomIndex = sPointPos;
        }
    }

    iter_swap( m_vectorPoints.begin(), m_vectorPoints.begin() + sMostLeftBottomIndex );
}

void ConvexHullCalculator::calculateConvexHull()
{
    m_vectorConvexHull.clear();

    sort( m_vectorPoints.begin() + 1, m_vectorPoints.end(), [this]( const Point &pointLhs, const Point &pointRhs )
    {
        return Point::compare( m_pointMostLeftBottom, pointLhs, pointRhs );
    } );

    for ( size_t sActualPos = 1; sActualPos < m_vectorPoints.size() - 1; )
    {
        if ( Point::getOrientation( m_pointMostLeftBottom, m_vectorPoints[sActualPos], m_vectorPoints[sActualPos + 1] ) == COLLINEAR )
            m_vectorPoints.erase( m_vectorPoints.begin() + sActualPos );
        else
            ++sActualPos;
    }

    if ( m_vectorPoints.size() < 3 )
        return;

    m_vectorConvexHull.push_back( m_vectorPoints[0] );
    m_vectorConvexHull.push_back( m_vectorPoints[1] );
    m_vectorConvexHull.push_back( m_vectorPoints[2] );

    for ( size_t sPosToInsert = 3; sPosToInsert < m_vectorPoints.size(); sPosToInsert++ )
    {
        size_t sConvexHullSize = m_vectorConvexHull.size();
        while ( Point::getOrientation( m_vectorConvexHull[sConvexHullSize - 2], m_vectorConvexHull[sConvexHullSize - 1], m_vectorPoints[sPosToInsert] ) !=
                2 )
        {
            m_vectorConvexHull.erase( m_vectorConvexHull.end() - 1 );
            --sConvexHullSize;
        }
        m_vectorConvexHull.push_back( m_vectorPoints[sPosToInsert] );
    }
}

double ConvexHullCalculator::calculateConvexHullArea()
{
    double dSumArea = 0.0;

    for ( size_t sPos = 1; sPos + 1 < m_vectorConvexHull.size(); ++sPos )
    {
        Point pointFirstSide = m_vectorConvexHull[sPos] - m_pointMostLeftBottom;
        Point pointSecondSide = m_vectorConvexHull[sPos + 1] - m_pointMostLeftBottom;

        int iCrossProductMagnitude = pointFirstSide % pointSecondSide;
        dSumArea += iCrossProductMagnitude / 2.0;
    }

    return dSumArea;
}

int main()
{
    size_t sNumberOfBacons;
    cin >> sNumberOfBacons;

    int iX;
    int iY;
    while ( sNumberOfBacons != 0 )
    {
        vector<Point> vectorPoints( sNumberOfBacons );
        for ( size_t sPos = 0; sPos < sNumberOfBacons; ++sPos )
        {
            cin >> iX >> iY;
            vectorPoints[sPos] = Point{ iX, iY };
        }

        ConvexHullCalculator convexHullCalculator( vectorPoints );
        convexHullCalculator.calculateConvexHull();
        cout << convexHullCalculator.calculateConvexHullArea() << endl;

        cin >> sNumberOfBacons;
    }

    return 0;
}