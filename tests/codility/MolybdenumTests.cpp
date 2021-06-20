#include <catch2/catch.hpp>

#include "Molybdenum.hpp"

struct TestInfo {
  std::vector<int> A;
  int K;
  std::vector<int> expectedLeaders;
};

TEST_CASE("General") {
  auto testInfo =
      // NOLINTNEXTLINE(readability-magic-numbers,google-build-using-namespace)
      GENERATE(TestInfo{{2, 1, 3, 1, 2, 2, 3}, 3, {2, 3}}, TestInfo{{1, 2, 2, 1, 2}, 4, {2, 3}}, TestInfo{{1}, 1, {2}});
  const auto calculatedLeaders = Molybdenum::solution(testInfo.A, testInfo.K);
  CHECK(calculatedLeaders == testInfo.expectedLeaders);
}
