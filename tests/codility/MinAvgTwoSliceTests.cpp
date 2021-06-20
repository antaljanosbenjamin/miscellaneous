#include <catch2/catch.hpp>

#include "MinAvgTwoSlice.hpp"

TEST_CASE("General") {
  // NOLINTNEXTLINE(readability-magic-numbers)
  std::vector<int> A{4, 2, 2, 5, 1, 5, 8};
  constexpr auto expectedMinAverage = 1;
  const auto calculatedMinAvarage = MinAvgTwoSlice::solution(A);
  CHECK(calculatedMinAvarage == expectedMinAverage);
}
