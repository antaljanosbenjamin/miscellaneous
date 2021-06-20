#include <catch2/catch.hpp>

#include "MinAbsSumOfTwo.hpp"

struct TestInfo {
  std::vector<int> A;
  int expectedSumOfTwo;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(google-build-using-namespace)
  auto testInfo = GENERATE(TestInfo{{1, 4, -3}, 1}, TestInfo{{-8, 4, 5, -10, 3}, 3}, TestInfo{{-8, 4, 5, -4, 3}, 0},
                           TestInfo{{-8, 4, 4, 4, 3}, 4}, TestInfo{{1}, 2}, TestInfo{{1, 2}, 2});
  const auto calculatedSumOfTwo = MinAbsSumOfTwo::solution(testInfo.A);
  CHECK(calculatedSumOfTwo == testInfo.expectedSumOfTwo);
}
