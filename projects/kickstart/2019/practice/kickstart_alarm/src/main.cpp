#include <iostream>

#include "KickStartAlarm.hpp"

using MyInt = uint64_t;

const MyInt resultModulo{1000000007U};

int main() {
  MyInt T{0};
  std::cin >> T;
  for (auto testCase{1U}; testCase <= T; ++testCase) {
    MyInt N{0};
    MyInt K{0};
    MyInt x1{0};
    MyInt y1{0};
    MyInt C{0};
    MyInt D{0};
    MyInt E1{0};
    MyInt E2{0};
    MyInt F{0};

    std::cin >> N >> K >> x1 >> y1 >> C >> D >> E1 >> E2 >> F;

    MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);

    std::cout << "Case #" << testCase << ": " << POWER_K << '\n';
  }
}
