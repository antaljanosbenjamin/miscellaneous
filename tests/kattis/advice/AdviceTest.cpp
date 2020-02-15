#include <sstream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "Advice.hpp"
#include "Point.hpp"

bool checkInput(const size_t &pathCount, const std::string &pathes, const Point &expectedAverage,
                const double &expectedLongestDistance) {
  std::stringstream ss;
  ss << pathes;
  AdviceCalculator calculator{pathCount};
  calculator.readDatas(ss);
  auto pointAverage = calculator.getAverageDestination();
  CHECK(pointAverage.x == Approx(expectedAverage.x));
  CHECK(pointAverage.y == Approx(expectedAverage.y));
  CHECK(calculator.getLongestDistance() == Approx(expectedLongestDistance));
  return false;
};

TEST_CASE("Simple route") {
  constexpr auto pathCount = 1u;
  const auto pathes = "87.342 34.30 start 0 walk 10.0\n";
  constexpr Point expectedAverage{97.342, 34.30};
  constexpr auto expectedLongestDistance = 0.0;
  REQUIRE_NOTHROW(checkInput(pathCount, pathes, expectedAverage, expectedLongestDistance));
};

TEST_CASE("Long route") {
  constexpr auto pathCount = 1u;
  const auto pathes =
      "87.342 34.30 start 0 walk 10.0 walk 40 turn 40.0 walk 60  walk 50 turn 90 walk 40 turn 13 walk 5\n";
  constexpr Point expectedAverage{191.9022, 138.6574};
  constexpr auto expectedLongestDistance = 0.0;
  REQUIRE_NOTHROW(checkInput(pathCount, pathes, expectedAverage, expectedLongestDistance));
};

TEST_CASE("Two routes") {
  constexpr auto pathCount = 2u;
  const auto pathes = "30 40 start 90 walk 5\n"
                      "40 50 start 180 walk 10 turn 90 walk 5\n";
  constexpr Point expectedAverage{30, 45};
  constexpr auto expectedLongestDistance = 0;
  REQUIRE_NOTHROW(checkInput(pathCount, pathes, expectedAverage, expectedLongestDistance));
};

TEST_CASE("Three routes") {
  constexpr auto pathCount = 3u;
  const auto pathes = "87.342 34.30 start 0 walk 10.0\n"
                      "2.6762 75.2811 start -45.0 walk 40 turn 40.0 walk 60\n"
                      "58.518 93.508 start 270 walk 50 turn 90 walk 40 turn 13 walk 5\n";
  constexpr Point expectedAverage{97.1547, 40.2334};
  constexpr auto expectedLongestDistance = 7.63097;
  REQUIRE_NOTHROW(checkInput(pathCount, pathes, expectedAverage, expectedLongestDistance));
};