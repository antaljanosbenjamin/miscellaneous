#include <catch2/catch.hpp>

#include "MinAbsSum.hpp"

TEST_CASE("General") {
  // NOLINTNEXTLINE(readability-magic-numbers)
  std::vector<int> A{1, 5, 2, -2};
  constexpr auto expectedSum = 0;
  const auto calculatedSum = MinAbsSum::solution(A);
  CHECK(calculatedSum == expectedSum);
}
