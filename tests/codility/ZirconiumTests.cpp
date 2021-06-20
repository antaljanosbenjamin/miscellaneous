#include <catch2/catch.hpp>

#include "Zirconium.hpp"

struct TestInfo {
  std::vector<int> A;
  std::vector<int> B;
  int F;
  int maximumSumOfContributions;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(google-build-using-namespace)
  auto testInfo = GENERATE(TestInfo{{4, 2, 1}, {2, 5, 3}, 2, 10}, TestInfo{{7, 1, 4, 4}, {5, 3, 4, 3}, 2, 18},
                           TestInfo{{5, 5, 5}, {5, 5, 5}, 1, 15});
  const auto calculatedSumOfContributions = Zirconium::solution(testInfo.A, testInfo.B, testInfo.F);
  CHECK(calculatedSumOfContributions == testInfo.maximumSumOfContributions);
}
