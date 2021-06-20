#include <catch2/catch.hpp>

#include "Ruthenium.hpp"

struct TestInfo {
  std::vector<int> A;
  int K;
  int expectedConsecutiveBooks;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(google-build-using-namespace)
  auto testInfo = GENERATE(TestInfo{{1, 1, 3, 4, 3, 3, 4}, 2, 5}, TestInfo{{4, 5, 5, 4, 2, 2, 4}, 0, 2},
                           TestInfo{{1, 3, 3, 2}, 2, 4}, TestInfo{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 10, 10});
  const auto calculatedConsecutiveBooks = Ruthenium::solution(testInfo.A, testInfo.K);
  CHECK(calculatedConsecutiveBooks == testInfo.expectedConsecutiveBooks);
}
