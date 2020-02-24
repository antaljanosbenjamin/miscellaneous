#include <iostream>
#include <numeric>
#include <vector>

using MyInt = uint64_t;

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

    const auto powerModulo{1000000007u};

    auto modPower = [&powerModulo](const auto val) { return val % powerModulo; };

    auto quickPowerWithModulo = [&modPower](auto base, auto exp) {
      auto t{1u};
      while (exp != 0) {
        if (exp % 2 == 1)
          t = modPower(t * base);
        base = modPower(base * base);
        exp /= 2;
      }
      return t;
    };

    auto moduloInverse = [&powerModulo, &quickPowerWithModulo](const auto val) {
      return quickPowerWithModulo(val, powerModulo - 2u);
    };

    MyInt POWER{0u};
    MyInt totalSumOfGeometricProgressions{K};
    // POWER_K(A) = power_1(A) + power_2(A) + ... + power_K(A) =
    // = (N + 1 - 1)(1^1)A_1 + (N + 1 - 2)(1^1 + 2^1)A_2 + ... (N + 1 - N)(1^1 + 2^1 + ... + N^1)A_N +
    // + (N + 1 - 1)(1^2)A_1 + (N + 1 - 2)(1^2 + 2^2)A_2 + ... (N + 1 - N)(1^2 + 2^2 + ... + N^2)A_N +
    // + ... +
    // + (N + 1 - 1)(1^K)A_1 + (N + 1 - 2)(1^K + 2^K)A_2 + ... (N + 1 - N)(1^K + 2^K + ... + N^K)A_N =
    // = (N + 1 - 1)(1^1 + 1^2 + ... + 1^K)A_1 +
    // + (N + 1 - 2)(1^1 + 1^2 + ... + 1^K + 2^1 + 2^2 + ... + 2^K)A_2 +
    // + ... +
    // + (N + 1 - N)(1^1 + 1^2 + ... + 1^K + 2^1 + 2^2 + ... + 2^K + ... + N^1 + N^2 + ... + N^K)A_N
    for (MyInt x{1u}; x <= N; ++x) {
      if (x != 1u) {
        // Sum of geometric progression
        // x^1 + x^2 + ... + x^K =
        // = x(1 - x^K)/(1 - x) =
        // = (x - x^(K + 1))/(1 - x) =
        // = (x^(K+1) - x)/(x - 1) =
        // = (x^(K+1) - x)(x - 1)^(-1)
        //   |      t1   ||     t2   |
        const auto t1 = powerModulo + quickPowerWithModulo(x, K + 1u) - x;
        // Calculate modular inverse using Euler-theorem
        // (x - 1)^(-1) mod M = (x - 1)^(M - 2) mod M
        const auto t2 = moduloInverse(MyInt{x - 1u});
        const auto sumOfSingleGeometricProgression = modPower(t1 * t2);
        totalSumOfGeometricProgressions = modPower(totalSumOfGeometricProgressions + sumOfSingleGeometricProgression);
      }
      POWER = modPower(POWER + modPower(modPower(totalSumOfGeometricProgressions * A[x - 1u]) * (N + 1u - x)));
    }

    std::cout << "Case #" << testCase << ": " << POWER << '\n';
  }
}
