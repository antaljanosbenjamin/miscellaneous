#include <catch2/catch.hpp>

#include "Niobium.hpp"

struct TestInfo {
  std::vector<std::vector<int>> A;
  int expectedMaxRows;
};

TEST_CASE("General") {
  // NOLINTNEXTLINE(google-build-using-namespace)
  auto testInfo = GENERATE(TestInfo{{
                                        {0, 0, 0, 0},
                                        {0, 1, 0, 0},
                                        {1, 0, 1, 1},
                                    },
                                    2},
                           TestInfo{{
                                        {0, 1, 0, 1},
                                        {1, 0, 1, 0},
                                        {0, 1, 0, 1},
                                        {1, 0, 1, 0},
                                    },
                                    4});
  const auto calculatedMaxRows = Niobium::solution(testInfo.A);
  CHECK(calculatedMaxRows == testInfo.expectedMaxRows);
}
