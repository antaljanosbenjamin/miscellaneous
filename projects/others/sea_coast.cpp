#include <iostream>
#include <queue>
#include <vector>

// Calculates the sea coast' length of some island. There are land and water fields. A water field is considered sea
// when it is adjacent to the sea. The sea surrounds the map, so water fields at the edge of map are considered to sea
// and land fields at the edge of the map are considered sea coasts. Two fields are adjacent if they have a common edge.
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
// On the map above the O means sea, W means water that isn't sea, and X means land. The - and | marks are
// one-unit-length sea coast.
//    - -
//  O|X X|O
//        -
//  O|X W X|
//  - -   -
// |X|O|X|O
//      -
// |X|O O O
//  -

enum FieldType { WATER, LAND, SEA };

struct CoordOffset {
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  int iRowOffset;
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  int iColOffset;
};

struct Coord {
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  size_t sRow;
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  size_t sCol;

  Coord operator+(const CoordOffset &rhs) const;
};

class SeaCoastCounter {
public:
  void readInput(std::istream &inputSteam);

  unsigned int calculateSeaCoastLength();

private:
  static FieldType getFieldType(char cField);

  FieldType &fieldAt(const Coord &coordField);

  void initializeCoordsToBecameSea();

  void countSeaCoastOneTheEdge();

  void handleBecameSea(const Coord &coordNewSea);

  void processCoordsToBecameSea();

private:
  size_t m_sRows{0U};
  size_t m_sCols{0U};
  size_t m_sRowsInside{0U};
  size_t m_sColsInside{0U};
  std::vector<std::vector<FieldType>> m_vectorMap{};
  std::queue<Coord> m_queueCoordsToBecameSea{};
  unsigned int uiSeaCoastLength{0U};

  static const std::vector<CoordOffset> &getNeighbourOffsets();
};

const std::vector<CoordOffset> &SeaCoastCounter::getNeighbourOffsets() {
  static const std::vector<CoordOffset> vectorNeighbourOffsets{CoordOffset{1, 0}, CoordOffset{0, -1},
                                                               CoordOffset{-1, 0}, CoordOffset{0, 1}};
  return vectorNeighbourOffsets;
}

Coord Coord::operator+(const CoordOffset &rhs) const {
  int iNewRow = static_cast<int>(sRow) + rhs.iRowOffset;
  int iNewCol = static_cast<int>(sCol) + rhs.iColOffset;

  return Coord{static_cast<size_t>(iNewRow), static_cast<size_t>(iNewCol)};
}

FieldType SeaCoastCounter::getFieldType(char cField) {
  switch (cField) {
  case '0':
    return WATER;
  case '1':
    return LAND;
  default:
    std::cerr << "Undefined field type: ";
    std::cerr << cField << std::endl;
    return WATER;
  }
}

FieldType &SeaCoastCounter::fieldAt(const Coord &coordField) {
  return m_vectorMap[coordField.sRow][coordField.sCol];
}

void SeaCoastCounter::readInput(std::istream &inputSteam) {
  inputSteam >> m_sRowsInside >> m_sColsInside;
  m_sRows = m_sRowsInside + 2;
  m_sCols = m_sColsInside + 2;

  m_vectorMap = std::vector<std::vector<FieldType>>(m_sRows, std::vector<FieldType>(m_sCols, SEA));

  char cField;
  for (size_t sActualRow = 1; sActualRow < m_sRows - 1; ++sActualRow) {
    for (size_t sActualCol = 1; sActualCol < m_sCols - 1; ++sActualCol) {
      inputSteam >> cField;
      m_vectorMap[sActualRow][sActualCol] = getFieldType(cField);
    }
  }
}

void SeaCoastCounter::initializeCoordsToBecameSea() {
  for (size_t sActualRow = 1; sActualRow <= m_sRowsInside; ++sActualRow) {
    if (m_vectorMap[sActualRow][1] == WATER) {
      m_queueCoordsToBecameSea.push({sActualRow, 1});
    }

    if (m_vectorMap[sActualRow][m_sColsInside] == WATER) {
      m_queueCoordsToBecameSea.push({sActualRow, m_sColsInside});
    }
  }

  for (size_t sActualCol = 2; sActualCol < m_sColsInside; ++sActualCol) {
    if (m_vectorMap[1][sActualCol] == WATER) {
      m_queueCoordsToBecameSea.push({1, sActualCol});
    }

    if (m_vectorMap[m_sRowsInside][sActualCol] == WATER) {
      m_queueCoordsToBecameSea.push({m_sRowsInside, sActualCol});
    }
  }
}

void SeaCoastCounter::countSeaCoastOneTheEdge() {
  for (size_t sActualRow = 1; sActualRow <= m_sRowsInside; ++sActualRow) {
    if (m_vectorMap[sActualRow][1] == LAND) {
      ++uiSeaCoastLength;
    }

    if (m_vectorMap[sActualRow][m_sColsInside] == LAND) {
      ++uiSeaCoastLength;
    }
  }

  for (size_t sActualCol = 1; sActualCol <= m_sColsInside; ++sActualCol) {
    if (m_vectorMap[1][sActualCol] == LAND) {
      ++uiSeaCoastLength;
    }

    if (m_vectorMap[m_sRowsInside][sActualCol] == LAND) {
      ++uiSeaCoastLength;
    }
  }
}

void SeaCoastCounter::handleBecameSea(const Coord &coordNewSea) {
  if (fieldAt(coordNewSea) == SEA) {
    return;
  }
  m_vectorMap[coordNewSea.sRow][coordNewSea.sCol] = SEA;

  for (const CoordOffset &coordOffsetNeighbour: getNeighbourOffsets()) {
    Coord coordNeighbour = coordNewSea + coordOffsetNeighbour;

    if (fieldAt(coordNeighbour) == LAND) {
      ++uiSeaCoastLength;
    } else if (fieldAt(coordNeighbour) == WATER) {
      m_queueCoordsToBecameSea.push(coordNeighbour);
    }
  }
}

void SeaCoastCounter::processCoordsToBecameSea() {
  while (m_queueCoordsToBecameSea.empty() == false) {
    Coord coordActual = m_queueCoordsToBecameSea.front();
    m_queueCoordsToBecameSea.pop();

    handleBecameSea(coordActual);
  }
}

unsigned int SeaCoastCounter::calculateSeaCoastLength() {
  uiSeaCoastLength = 0;
  m_queueCoordsToBecameSea = std::queue<Coord>();

  initializeCoordsToBecameSea();
  countSeaCoastOneTheEdge();

  processCoordsToBecameSea();
  return uiSeaCoastLength;
}

int main() {
  SeaCoastCounter seaCoastComputer;
  seaCoastComputer.readInput(std::cin);
  std::cout << seaCoastComputer.calculateSeaCoastLength() << "\n";

  return 0;
}
