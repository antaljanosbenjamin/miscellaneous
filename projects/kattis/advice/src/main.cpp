#include <iostream>
#include <limits>

#include "Advice.hpp"
#include "Point.hpp"

int main() {
  size_t sPathCount{0};
  std::cin >> sPathCount;
  while (sPathCount > 0) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    AdviceCalculator adviceCalculator(sPathCount);
    adviceCalculator.readDatas(std::cin);
    adviceCalculator.calculate();
    Point pointAverage = adviceCalculator.getAverageDestination();
    std::cout << pointAverage.x << " " << pointAverage.y << " " << adviceCalculator.getLongestDistance() << "\n";
    std::cin >> sPathCount;
  }
}
