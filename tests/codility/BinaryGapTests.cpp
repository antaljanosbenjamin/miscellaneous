#include <catch2/catch.hpp>

#include "BinaryGap.hpp"

struct TestInfo {
  int N;
  int expectedBinaryGap;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(readability-magic-numbers,google-build-using-namespace)
  const auto &testInfo = GENERATE(TestInfo{9, 2}, TestInfo{529, 4}, TestInfo{20, 1}, TestInfo{15, 0}, TestInfo{32, 0});
  const auto calculatedBinaryGap = BinaryGap::solution(testInfo.N);
  CHECK(calculatedBinaryGap == testInfo.expectedBinaryGap);
}
