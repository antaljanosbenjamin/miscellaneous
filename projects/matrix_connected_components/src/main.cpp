
#include <iostream>

#include "Algorithm.hpp"
#include "utils/Matrix.hpp"

int main() {
  static constexpr size_t width{20};
  static constexpr size_t height{10};
  utils::Matrix<uint8_t> m{width, height, 2};

  for (auto row = 0U; row < height; ++row) {
    for (auto column = 0U; column < width; ++column) {
      m.get(row, column) = row * width + column;
    }
  }
  for (auto row = 0U; row < height; ++row) {
    for (auto column = 0U; column < width; ++column) {
      std::cout << static_cast<int>(m.get(row, column)) << ' ';
    }
    std::cout << std::endl;
  }

  matrix_connected_components::doMagic(m);
  return 0;
}