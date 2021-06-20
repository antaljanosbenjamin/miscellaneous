#include <catch2/catch.hpp>

#include "MaxProfit.hpp"

TEST_CASE("General") {
  std::vector<int> A{23171, 21011, 21123, 21366, 21013, 21367}; // NOLINT(readability-magic-numbers)
  const auto expectedProfit = 356;
  const auto calcualtedMaxProfit = MaxProfit::solution(A);
  CHECK(calcualtedMaxProfit == expectedProfit);
}
