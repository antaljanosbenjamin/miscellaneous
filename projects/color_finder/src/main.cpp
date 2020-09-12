#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

#include "ColorFinder.hpp"

int main(const int argc, const char **argv) {
  const auto getColorComponent = [](const char *str) {
    constexpr int minValue = std::numeric_limits<uint8_t>::min();
    constexpr int maxValue = std::numeric_limits<uint8_t>::max();
    return static_cast<uint8_t>(std::clamp(std::stoi(str), minValue, maxValue));
  };
  if (argc != 4) {
    std::cerr << "Please provide the three components in range [0,255]!\n";
    return 1;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ColorFinder::Color c{getColorComponent(argv[1]), getColorComponent(argv[2]), getColorComponent(argv[3])};
  std::cout << getClosestColorName(c) << std::endl;
  return 0;
}