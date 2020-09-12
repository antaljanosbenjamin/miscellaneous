#include <algorithm>
#include <cassert>
#include <iostream>

#include "ColorFinder.hpp"

int main(const int argc, const char **argv) {
  const auto getColorComponent = [](const char *str) {
    return static_cast<uint8_t>(std::clamp(std::stoi(str), 0, 255));
  };
  if (argc != 4) {
    std::cerr << "Please provide the three components in range [0,255]!\n";
    return 1;
  }

  ColorFinder::Color c{getColorComponent(argv[1]), getColorComponent(argv[2]), getColorComponent(argv[3])};
  std::cout << getClosestColorName(c) << std::endl;
  return 0;
}