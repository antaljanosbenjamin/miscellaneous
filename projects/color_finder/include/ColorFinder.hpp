#pragma once

#include <cmath>
#include <vector>

#include "ColorFinderConstexpr.h"
#include "Colors.hpp"

namespace ColorFinder {

CF_CONSTEXPR std::string_view getClosestColorName(const Color &color) {
  const auto sumOfDifference = [](const Color &lhs, const Color &rhs) {
    const auto absValue = [](const int value) { return value < 0 ? -value : value; };
    return absValue(lhs.r - rhs.r) + absValue(lhs.g - rhs.g) + absValue(lhs.b - rhs.b);
  };
  int smallestDifference = sumOfDifference(color, colors[0].color);
  std::string_view closestColorName = colors[0].name;
  for (const auto &colorInfo: colors) {
    const auto difference = sumOfDifference(color, colorInfo.color);
    if (difference < smallestDifference) {
      smallestDifference = difference;
      closestColorName = colorInfo.name;
    }
  }
  return closestColorName;
}
} // namespace ColorFinder
