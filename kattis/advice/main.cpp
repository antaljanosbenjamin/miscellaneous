#include <iostream>
#include <limits>

#include "Advice.hpp"
#include "Point.hpp"

using namespace std;

int main() {
  size_t sPathCount;
  cin >> sPathCount;
  while (sPathCount > 0) {
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    AdviceCalculator adviceCalculator(sPathCount);
    adviceCalculator.readDatas(cin);
    adviceCalculator.calculate();
    Point pointAverage = adviceCalculator.getAverageDestination();
    std::cout << pointAverage.x << " " << pointAverage.y << " " << adviceCalculator.getLongestDistance() << endl;
    cin >> sPathCount;
  }
}
