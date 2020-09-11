#include <cassert>
#include <iostream>

#include "ColorFinder.hpp"

int main() {
  constexpr ColorFinder::Color c{0x82, 0x12, 0x18};
  constexpr auto name = getClosestColorName(c);
  static_assert(name == "Falu Red", "Something went wrong");
  std::cout << name << std::endl;
  return 0;
}