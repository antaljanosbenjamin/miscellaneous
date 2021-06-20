#include <catch2/catch.hpp>

#include "MaxSliceSum.hpp"

TEST_CASE("General") {
  std::vector<int> A{3, 2, -6, 4, 0}; // NOLINT(readability-magic-numbers)
  const auto expectedSum = 5;
  const auto calculatedSum = MaxSliceSum::solution(A);
  CHECK(calculatedSum == expectedSum);
}
