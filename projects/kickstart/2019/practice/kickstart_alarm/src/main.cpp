#include <iostream>
#include "KickStartAlarm.hpp"

using MyInt = uint64_t;

MyInt resultModulo = 1000000007;

int main() {
  MyInt T;
  std::cin >> T;
  for (auto testCase{1u}; testCase <= T; ++testCase) {
    MyInt N;
    MyInt K;
    MyInt x1;
    MyInt y1;
    MyInt C;
    MyInt D;
    MyInt E1;
    MyInt E2;
    MyInt F;

    std::cin >> N >> K >> x1 >> y1 >> C >> D >> E1 >> E2 >> F;

    MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);

    std::cout << "Case #" << testCase << ": " << POWER_K << '\n';
  }
}
