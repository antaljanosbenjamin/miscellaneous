#include <catch2/catch.hpp>

#include "MaxDoubleSliceSum.hpp"

struct TestInfo {
  std::vector<int> A;
  int expectedDoubleSliceSum;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(readability-magic-numbers,google-build-using-namespace)
  auto testInfo = GENERATE(TestInfo{{3, 2, 6, -1, 4, 5, -1, 2}, 17}, TestInfo{{3, 2, 6, -1, 4, 5, -1, 2}, 17});
  const auto calcualtedDoubleSliceSum = MaxDoubleSliceSum::solution(testInfo.A);
  CHECK(calcualtedDoubleSliceSum == testInfo.expectedDoubleSliceSum);
}
