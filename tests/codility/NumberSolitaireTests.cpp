#include <catch2/catch.hpp>

#include "NumberSolitaire.hpp"

TEST_CASE("General") {
  std::vector<int> A{1, -2, 0, 9, -1, -2}; // NOLINT(readability-magic-numbers)
  constexpr auto expectedMaxResult = 8;
  const auto calculatedMaxResult = NumberSolitaire::solution(A);
  CHECK(calculatedMaxResult == expectedMaxResult);
}
