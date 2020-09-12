#include <sstream>

#include <catch2/catch.hpp>

#include "ColorFinder.hpp"
#include "ColorFinderConstexpr.h"

#define CHECK_COLOR_NAME_RUNTIME(ExpectedNameLiteral, ActualName)                                                      \
  CHECK(std::string(ExpectedNameLiteral) == std::string(ActualName));

#if CF_USE_CONSTEXPR
#define CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, ActualName)                                                    \
  static_assert(std::string_view(ExpectedNameLiteral) == ActualName, "Name doesn't match");
#else
#define CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, ActualName)
#endif

#define CHECK_COLOR_NAME(ExpectedNameLiteral, ActualName)                                                              \
  CHECK_COLOR_NAME_RUNTIME(ExpectedNameLiteral, ActualName)                                                            \
  CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, ActualName)

TEST_CASE("Exact match: Black") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{0U, 0U, 0U});
  CHECK_COLOR_NAME("Black", name);
}

TEST_CASE("Exact match: Black Hole") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{1U, 2U, 3U});
  CHECK_COLOR_NAME("Black Hole", name);
}

TEST_CASE("Exact match: Abalone") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{248U, 243U, 246U});
  CHECK_COLOR_NAME("Abalone", name);
}

TEST_CASE("Above: Dusk Blue") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{124U, 160U, 192U});
  CHECK_COLOR_NAME("Dusk Blue", name);
}

TEST_CASE("Above: Java") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{38U, 152U, 152U});
  CHECK_COLOR_NAME("Java", name);
}

TEST_CASE("Below: Earth") {
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{161U, 100U, 61U});
  CHECK_COLOR_NAME("Earth", name);
}