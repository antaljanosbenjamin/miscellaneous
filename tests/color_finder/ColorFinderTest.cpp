#include <sstream>

#include <catch2/catch.hpp>

#include "ColorFinder.hpp"
#include "ColorFinderConstexpr.h"

#if CF_USE_CONSTEXPR
  // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
  #define CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, ActualName)                                                  \
    static_assert(std::string_view(ExpectedNameLiteral) == (ActualName), "Name doesn't match");
#else
  #define CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, ActualName)
#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_COLOR_NAME(ExpectedNameLiteral, ActualName)                                                              \
  CHECK(std::string(ExpectedNameLiteral) == std::string(ActualName));                                                  \
  CHECK_COLOR_NAME_CONSTEXPR(ExpectedNameLiteral, (ActualName))

TEST_CASE("Exact match: Black") {
  constexpr auto blackComponentsValue = uint8_t{0x00};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(
      ColorFinder::Color{blackComponentsValue, blackComponentsValue, blackComponentsValue});
  CHECK_COLOR_NAME("Black", name);
}

TEST_CASE("Exact match: Black Hole") {
  constexpr auto blackHoleComponents = std::array<uint8_t, 3>{0x01, 0x02, 0x03};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(
      ColorFinder::Color{blackHoleComponents[0], blackHoleComponents[1], blackHoleComponents[2]});
  CHECK_COLOR_NAME("Black Hole", name);
}

TEST_CASE("Exact match: Abalone") {
  constexpr auto abaloneComponents = std::array<uint8_t, 3>{0xf8, 0xf3, 0xf6};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(
      ColorFinder::Color{abaloneComponents[0], abaloneComponents[1], abaloneComponents[2]});
  CHECK_COLOR_NAME("Abalone", name);
}

TEST_CASE("Above: Dusk Blue") {
  constexpr auto duskBlueComponents = std::array<uint8_t, 3>{0x7b, 0xa0, 0xc0};
  constexpr auto increment = uint8_t{1};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(
      ColorFinder::Color{duskBlueComponents[0] + increment, duskBlueComponents[1], duskBlueComponents[2]});
  CHECK_COLOR_NAME("Dusk Blue", name);
}

TEST_CASE("Above: Java") {
  constexpr auto javaComponents = std::array<uint8_t, 3>{0x25, 0x97, 0x97};
  constexpr auto increment = uint8_t{1};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(
      ColorFinder::Color{javaComponents[0] + increment, javaComponents[1] + increment, javaComponents[2] + increment});
  CHECK_COLOR_NAME("Java", name);
}

TEST_CASE("Below: Earth") {
  constexpr auto earthComponents = std::array<uint8_t, 3>{0xa2, 0x65, 0x3e};
  constexpr auto decrement = uint8_t{1};
  CF_CONSTEXPR auto name = ColorFinder::getClosestColorName(ColorFinder::Color{
      earthComponents[0] - decrement, earthComponents[1] - decrement, earthComponents[2] - decrement});
  CHECK_COLOR_NAME("Earth", name);
}