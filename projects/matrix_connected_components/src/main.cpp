
#include <iostream>

#include <chrono>
#include <random>

#include "Algorithm.hpp"
#include "utils/Matrix.hpp"

int main() {
  // static constexpr int64_t width{6};
  // static constexpr int64_t height{5};
  // utils::Matrix<uint8_t> m{height, width, 0};

  // std::vector<std::pair<int64_t, int64_t>> markedFields = {
  //     {0, 0}, {1, 0}, {1, 2}, {1, 3}, {2, 1}, {2, 3}, {3, 1}, {3, 2}, {3, 3}, {4, 4},
  // };

  // for (const auto &[row, column]: markedFields) {
  //   m.get(row, column) = 1;
  // }
  // for (auto row = 0U; row < height; ++row) {
  //   for (auto column = 0U; column < width; ++column) {
  //     std::cout << static_cast<int>(m.get(row, column)) << ' ';
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;
  // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  // matrix_connected_components::doMagic(m, begin);

  static constexpr int64_t kDimension{20000};
  static constexpr int64_t kNumberOfFields{kDimension * kDimension};

  std::mt19937 mt(1); // NOLINTNEXTLINE(cert-msc32-c)
  std::uniform_int_distribution<int64_t> r1(0, kDimension - 1);
  utils::Matrix<uint8_t> matrix(kDimension, kDimension);

  static constexpr int64_t kIterations = kNumberOfFields / 5 * 2;
  for (auto i = 0; i < kIterations; ++i) {
    matrix.get(r1(mt), r1(mt)) = 1;
  }

  std::cout << "Start\n";
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  matrix_connected_components::doMagic(matrix);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Done\n";

  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
            << "[ms]" << std::endl;

  return 0;
}