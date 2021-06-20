#include <catch2/catch.hpp>

#include "Rhodium.hpp"

TEST_CASE("General") {
  std::vector T{2, 0, 2, 2, 1, 0};
  constexpr auto expectedNumberOfPossibleVacations = 12;
  const auto calculatedNumberOfPossibleVacations = Rhodium::solution(T);
  CHECK(calculatedNumberOfPossibleVacations == expectedNumberOfPossibleVacations);
}
