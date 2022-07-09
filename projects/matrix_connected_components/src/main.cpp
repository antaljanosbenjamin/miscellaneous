
#include <iostream>

#include <chrono>
#include <random>

#include "matrix_connected_components/Algorithm.hpp"
#include "utils/containers/Matrix.hpp"

template <typename T>
struct StringRep {};

template <>
struct StringRep<uint64_t> {
  static constexpr std::string_view value{"uint64_t"};
};

template <>
struct StringRep<int64_t> {
  static constexpr std::string_view value{"int64_t"};
};

int main() {
  // using ValueType = uint64_t;
  // static constexpr int64_t width{6};
  // static constexpr int64_t height{5};
  // utils::containers::Matrix<ValueType> m{height, width, matrix_connected_components::kUnmarkedField<ValueType>};

  // std::vector<std::pair<int64_t, int64_t>> markedFields = {
  //     {0, 0}, {1, 0}, {1, 2}, {1, 3}, {2, 1}, {2, 3}, {3, 1}, {3, 2}, {3, 3}, {4, 4},
  // };

  // for (const auto &[row, column]: markedFields) {
  //   m.get(row, column) = matrix_connected_components::kMarkedField<ValueType>;
  // }
  // for (auto row = 0U; row < height; ++row) {
  //   for (auto column = 0U; column < width; ++column) {
  //     std::cout << static_cast<ValueType>(m.get(row, column)) << ' ';
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;
  // matrix_connected_components::labelConnectedCompnents(m);
  // for (auto row = 0U; row < height; ++row) {
  //   for (auto column = 0U; column < width; ++column) {
  //     std::cout << static_cast<ValueType>(m.get(row, column)) << ' ';
  //   }
  //   std::cout << std::endl;
  // }

  static constexpr int64_t kDimension{20000};
  static constexpr int64_t kNumberOfFields{kDimension * kDimension};

  using ValueType = uint64_t;
  std::mt19937 mt(1); // NOLINTNEXTLINE(cert-msc32-c)
  std::uniform_int_distribution<int64_t> r1(0, kDimension - 1);
  utils::containers::Matrix<ValueType> matrix(kDimension, kDimension,
                                              matrix_connected_components::kUnmarkedField<ValueType>);

  static constexpr int64_t kIterations = kNumberOfFields / 5 * 2;
  for (auto i = 0; i < kIterations; ++i) {
    matrix.get(r1(mt), r1(mt)) = matrix_connected_components::kMarkedField<ValueType>;
  }

  std::cout << "Start " << StringRep<ValueType>::value << '\n';
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  const auto components = matrix_connected_components::countConnectedComponents(std::move(matrix));
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Done, there are " << components << " components.\n";

  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
            << "[ms]" << std::endl;

  return 0;
}
