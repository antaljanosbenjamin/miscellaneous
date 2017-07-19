#include <iostream>
#include <vector>
#include <queue>

// Calculates the sea coast' length of some island. There are land and water fields.
// A water field is considered sea when it is adjacent to the sea. The sea surrounds
// the map, so water fields at the edge of map are considered to sea and land fields
// at the edge of the map are considered sea coasts.
// Two fields are adjacent if they have a common edge.
// Example input:
// 4 4             | The size of map: row col
// 0 1 1 0         | 0 means water
// 0 1 0 1         | 1 means land
// 1 0 1 0
// 1 0 0 0
//
// Output:
// 18
//
// Explanation:
// On the map above the O means sea, W means water that isn't sea, and X means
// land. The - and | marks are one-unit-length sea coast.
//   - -
// O|X X|O
//       -
// O|X W X|
// - -   -
//|X|O|X|O
//     -
//|X|O O O
// -

enum FieldType
{
    WATER,
    LAND,
    SEA
};

struct CoordOffset
{
    int iRowOffset;
    int iColOffset;

    constexpr CoordOffset( int iRowOffset, int iColOffset );
};

struct Coord
{
    size_t sRow;
    size_t sCol;

    const Coord operator+( const CoordOffset &rhs ) const;
};


using namespace std;

class SeaCoastCounter
{
public:
    SeaCoastCounter();

    void readInput( istream &inputSteam );

    unsigned int calculateSeaCoastLength();

private:
    FieldType getFieldType( char cField );

    FieldType &fieldAt( const Coord &coordField );

    void initializeCoordsToBecameSea();

    void countSeaCoastOneTheEdge();

    void handleBecameSea( const Coord &coordNewSea );

    void processCoordsToBecameSea();

private:
    size_t m_sRows;
    size_t m_sCols;
    size_t m_sRowsInside;
    size_t m_sColsInside;
    vector<vector<FieldType>> m_vectorMap;
    queue<Coord> m_queueCoordsToBecameSea;
    unsigned int uiSeaCoastLength;

    static const vector<CoordOffset> ms_vectorNeighbourOffsets;
};

const vector<CoordOffset> SeaCoastCounter::ms_vectorNeighbourOffsets{ CoordOffset( 1, 0 ), CoordOffset( 0, -1 ), CoordOffset( -1, 0 ), CoordOffset( 0, 1 ) };

constexpr CoordOffset::CoordOffset( int iRowOffset, int iColOffset ) : iRowOffset( iRowOffset ), iColOffset( iColOffset )
{ }

const Coord Coord::operator+( const CoordOffset &rhs ) const
{
    int iNewRow = sRow + rhs.iRowOffset;
    int iNewCol = sCol + rhs.iColOffset;

    return Coord{ (size_t) iNewRow, (size_t) iNewCol };
}

SeaCoastCounter::SeaCoastCounter() : m_sRows( 0 ), m_sCols( 0 ), m_sRowsInside( 0 ), m_sColsInside( 0 ), m_vectorMap(), m_queueCoordsToBecameSea(),
                                       uiSeaCoastLength( 0 )
{ }

FieldType SeaCoastCounter::getFieldType( char cField )
{
    switch (cField)
    {
        case '0' :
            return WATER;
        case '1' :
            return LAND;
        default  :
            cerr << "Undefined field type: ";
            cerr << cField << endl;
            return WATER;
    }
}

FieldType &SeaCoastCounter::fieldAt( const Coord &coordField )
{
    return m_vectorMap[coordField.sRow][coordField.sCol];
}

void SeaCoastCounter::readInput( istream &inputSteam )
{
    cin >> m_sRowsInside >> m_sColsInside;
    m_sRows = m_sRowsInside + 2;
    m_sCols = m_sColsInside + 2;

    m_vectorMap = vector<vector<FieldType>>( m_sRows, vector<FieldType>( m_sCols, SEA ));

    char cField;
    for (size_t sActualRow = 1; sActualRow < m_sRows - 1; ++sActualRow)
        for (size_t sActualCol = 1; sActualCol < m_sCols - 1; ++sActualCol)
        {
            cin >> cField;
            m_vectorMap[sActualRow][sActualCol] = getFieldType( cField );
        }
}

void SeaCoastCounter::initializeCoordsToBecameSea()
{
    for (size_t sActualRow = 1; sActualRow <= m_sRowsInside; ++sActualRow)
    {
        if (m_vectorMap[sActualRow][1] == WATER)
            m_queueCoordsToBecameSea.push( { sActualRow, 1 } );

        if (m_vectorMap[sActualRow][m_sColsInside] == WATER)
            m_queueCoordsToBecameSea.push( { sActualRow, m_sColsInside } );
    }

    for (size_t sActualCol = 2; sActualCol < m_sColsInside; ++sActualCol)
    {
        if (m_vectorMap[1][sActualCol] == WATER)
            m_queueCoordsToBecameSea.push( { 1, sActualCol } );

        if (m_vectorMap[m_sRowsInside][sActualCol] == WATER)
            m_queueCoordsToBecameSea.push( { m_sRowsInside, sActualCol } );
    }
}

void SeaCoastCounter::countSeaCoastOneTheEdge()
{
    for (size_t sActualRow = 1; sActualRow <= m_sRowsInside; ++sActualRow)
    {
        if (m_vectorMap[sActualRow][1] == LAND)
            ++uiSeaCoastLength;

        if (m_vectorMap[sActualRow][m_sColsInside] == LAND)
            ++uiSeaCoastLength;
    }

    for (size_t sActualCol = 1; sActualCol <= m_sColsInside; ++sActualCol)
    {
        if (m_vectorMap[1][sActualCol] == LAND)
            ++uiSeaCoastLength;

        if (m_vectorMap[m_sRowsInside][sActualCol] == LAND)
            ++uiSeaCoastLength;
    }
}

void SeaCoastCounter::handleBecameSea( const Coord &coordNewSea )
{
    if (fieldAt( coordNewSea ) == SEA)
        return;
    m_vectorMap[coordNewSea.sRow][coordNewSea.sCol] = SEA;

    for (const CoordOffset &coordOffsetNeighbour : ms_vectorNeighbourOffsets)
    {
        Coord coordNeighbour = coordNewSea + coordOffsetNeighbour;

        if (fieldAt( coordNeighbour ) == LAND)
        {
            /*cout << "From: (" << coordNewSea.sRow << "," << coordNewSea.sCol << ") to (" << coordNeighbour.sRow << "," << coordNeighbour.sCol
            << ")" << endl;*/

            ++uiSeaCoastLength;
        }
        else if (fieldAt( coordNeighbour ) == WATER)
            m_queueCoordsToBecameSea.push( coordNeighbour );
    }
}

void SeaCoastCounter::processCoordsToBecameSea()
{
    while (m_queueCoordsToBecameSea.empty() == false)
    {
        Coord coordActual = m_queueCoordsToBecameSea.front();
        m_queueCoordsToBecameSea.pop();

        handleBecameSea( coordActual );
    }
}

unsigned int SeaCoastCounter::calculateSeaCoastLength()
{
    uiSeaCoastLength = 0;
    m_queueCoordsToBecameSea = queue<Coord>();

    initializeCoordsToBecameSea();
    countSeaCoastOneTheEdge();

    processCoordsToBecameSea();
    return uiSeaCoastLength;
}

int main()
{
    SeaCoastCounter seaCoastComputer;
    seaCoastComputer.readInput( cin );
    cout << seaCoastComputer.calculateSeaCoastLength() << endl;

    return 0;
}
