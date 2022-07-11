#include "MatrixUtils.hpp"

namespace matrix_connected_components::tests {

size_t asSizeT(const int64_t value) {
  return static_cast<size_t>(value);
}

utils::containers::Matrix<uint64_t> makeMatrix(const std::vector<std::vector<uint64_t>> &input,
                                               const uint64_t defaultValue) {
  const auto height = static_cast<int64_t>(input.size());
  const auto width = static_cast<int64_t>(input.front().size());

  utils::containers::Matrix<uint64_t> matrix{height, width, defaultValue};
  for (int64_t row{0}; row < height; ++row) {
    for (int64_t column{0}; column < width; ++column) {
      matrix.get(row, column) = input[asSizeT(row)][asSizeT(column)];
    }
  }
  return matrix;
}
} // namespace matrix_connected_components::tests
