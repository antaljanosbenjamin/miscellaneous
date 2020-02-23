#include <iostream>
#include <numeric>
#include <vector>

using MyInt = size_t;

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

    x1 %= F;
    y1 %= F;
    C %= F;
    D %= F;
    E1 %= F;
    E2 %= F;

    std::vector<MyInt> A;
    A.reserve(N);

    {
      auto modF = [&F](const auto val) { return val % F; };
      A.push_back(modF(x1 + y1));
      MyInt prevX{x1};
      MyInt prevY{y1};
      for (auto i{1u}; i < N; ++i) {
        const auto currentX = modF(C * prevX + D * prevY + E1);
        const auto currentY = modF(D * prevX + C * prevY + E2);
        A.push_back(modF(currentX + currentY));
        prevX = currentX;
        prevY = currentY;
      }
    }
    std::vector<MyInt> currentPowers;
    currentPowers.resize(N);
    std::iota(currentPowers.begin(), currentPowers.end(), 1u);

    auto modPower = [](const auto val) {
      const auto powerModulo{1000000007u};
      return val % powerModulo;
    };

    const auto getPOWER = [&N, &A, &currentPowers, &modPower]() {
      MyInt NDependentFactor{N + 1};
      MyInt itemIndexDependentFactor{0u};
      MyInt POWER{0u};
      for (auto itemIndex{0u}; itemIndex < N; ++itemIndex) {
        --NDependentFactor;
        itemIndexDependentFactor = modPower(itemIndexDependentFactor + currentPowers[itemIndex]);
        POWER = POWER + modPower(modPower(NDependentFactor * itemIndexDependentFactor) * A[itemIndex]);
      }
      return POWER;
    };

    auto totalPOWER{0u};
    for (auto i{1u}; i <= K; ++i) {
      const auto ithPOWER{getPOWER()};
      totalPOWER = modPower(totalPOWER + ithPOWER);
      for (auto j{1u}; j <= N; ++j) {
        currentPowers[j - 1] = modPower(currentPowers[j - 1] * j);
      }
    }
    std::cout << "Case #" << testCase << ": " << totalPOWER << '\n';
  }
}
