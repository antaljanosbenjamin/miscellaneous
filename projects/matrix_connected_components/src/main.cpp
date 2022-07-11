#include <iostream>

#include "matrix_connected_components/Algorithm.hpp"
#include "utils/containers/Matrix.hpp"

int main() {
  using ValueType = uint8_t;
  static constexpr int64_t width{5};
  static constexpr int64_t height{5};
  utils::containers::Matrix<ValueType> m{height, width, matrix_connected_components::kUnmarkedField<ValueType>};

  std::vector<std::pair<int64_t, int64_t>> markedFields = {
      {0, 0}, {1, 0}, {1, 2}, {1, 3}, {2, 1}, {2, 3}, {3, 1}, {3, 2}, {3, 3}, {4, 4},
  };

  for (const auto &[row, column]: markedFields) {
    m.get(row, column) = matrix_connected_components::kMarkedField<ValueType>;
  }

  std::cout << "Matrix before running the algorithm: \n";

  for (auto row = 0U; row < height; ++row) {
    for (auto column = 0U; column < width; ++column) {
      std::cout << static_cast<int>(m.get(row, column)) << ' ';
    }
    std::cout << std::endl;
  }
  const auto numberOfConnectedComponents = matrix_connected_components::countConnectedComponents(m);
  std::cout << "Number of connected components: " << numberOfConnectedComponents << '\n';

  m = matrix_connected_components::labelConnectedComponents(std::move(m));

  std::cout << "Matrix after running the algorithm: \n";
  for (auto row = 0U; row < height; ++row) {
    for (auto column = 0U; column < width; ++column) {
      std::cout << static_cast<int>(m.get(row, column)) << ' ';
    }
    std::cout << std::endl;
  }

  return 0;
}
