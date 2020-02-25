#pragma once

#include <vector>

namespace KickStartAlarm {

template <typename IntType>
static IntType quickPowerWithModulo(IntType base, IntType exp, IntType modulo) {
  auto mod = [modulo](const auto value) { return value % modulo; };

  IntType t{1u};
  while (exp != 0) {
    if (exp % 2 == 1)
      t = mod(t * base);
    base = mod(base * base);
    exp /= 2;
  }
  return t;
};

template <typename IntType>
static IntType calculatePOWER_K(IntType N, IntType K, IntType x1, IntType y1, IntType C, IntType D, IntType E1,
                                IntType E2, IntType F, IntType resultModulo) {
  x1 %= F;
  y1 %= F;
  C %= F;
  D %= F;
  E1 %= F;
  E2 %= F;

  std::vector<IntType> A;
  A.reserve(N);

  {
    auto modF = [&F](const auto val) { return val % F; };
    A.push_back(modF(x1 + y1));
    IntType prevX{x1};
    IntType prevY{y1};
    for (auto i{1u}; i < N; ++i) {
      const auto currentX = modF(C * prevX + D * prevY + E1);
      const auto currentY = modF(D * prevX + C * prevY + E2);
      A.push_back(modF(currentX + currentY));
      prevX = currentX;
      prevY = currentY;
    }
  }

  auto modResult = [&resultModulo](const auto val) { return val % resultModulo; };

  auto quickPowerWithResultModulo = [&resultModulo](auto base, auto exp) {
    return quickPowerWithModulo(base, exp, resultModulo);
  };

  auto moduloInverse = [&resultModulo, &quickPowerWithResultModulo](const auto val) {
    return quickPowerWithResultModulo(val, resultModulo - 2u);
  };

  IntType POWER_K{0u};
  IntType totalSumOfGeometricProgressions{K};
  // POWER_K(A) = power_1(A) + power_2(A) + ... + power_K(A) =
  // = (N + 1 - 1)(1^1)A_1 + (N + 1 - 2)(1^1 + 2^1)A_2 + ... (N + 1 - N)(1^1 + 2^1 + ... + N^1)A_N +
  // + (N + 1 - 1)(1^2)A_1 + (N + 1 - 2)(1^2 + 2^2)A_2 + ... (N + 1 - N)(1^2 + 2^2 + ... + N^2)A_N +
  // + ... +
  // + (N + 1 - 1)(1^K)A_1 + (N + 1 - 2)(1^K + 2^K)A_2 + ... (N + 1 - N)(1^K + 2^K + ... + N^K)A_N =
  // = (N + 1 - 1)(1^1 + 1^2 + ... + 1^K)A_1 +
  // + (N + 1 - 2)(1^1 + 1^2 + ... + 1^K + 2^1 + 2^2 + ... + 2^K)A_2 +
  // + ... +
  // + (N + 1 - N)(1^1 + 1^2 + ... + 1^K + 2^1 + 2^2 + ... + 2^K + ... + N^1 + N^2 + ... + N^K)A_N
  for (IntType x{1u}; x <= N; ++x) {
    if (x != 1u) {
      // Sum of geometric progression
      // x^1 + x^2 + ... + x^K =
      // = (1 - x^(K+1))/(1 - x) =
      // = (x^(K+1) - 1)/(x - 1) =
      // = (x^(K+1) - 1)(x - 1)^(-1)
      //   |      t1   ||     t2   |
      const auto t1 = modResult(quickPowerWithResultModulo(x, K + 1u) + resultModulo - 1);
      // Calculate modular inverse using Euler-theorem
      // (x - 1)^(-1) mod M = (x - 1)^(M - 2) mod M
      const auto t2 = moduloInverse(x - 1u);
      const auto sumOfSingleGeometricProgression = modResult(modResult(t1 * t2) + resultModulo - 1);
      totalSumOfGeometricProgressions = modResult(totalSumOfGeometricProgressions + sumOfSingleGeometricProgression);
    }
    POWER_K = modResult(POWER_K + modResult(modResult(totalSumOfGeometricProgressions * A[x - 1u]) * (N + 1u - x)));
  }
  return POWER_K;
}
}; // namespace KickStartAlarm