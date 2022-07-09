#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "matrix_connected_components/Algorithm.hpp"
#include "utils/containers/Matrix.hpp"

namespace matrix_connected_components::tests {

size_t asSizeT(const int64_t value) {
  return static_cast<size_t>(value);
}

utils::containers::Matrix<uint64_t> makeMatrix(const std::vector<std::string> &input) {
  const auto height = static_cast<int64_t>(input.size());
  const auto width = static_cast<int64_t>(input.front().size());

  utils::containers::Matrix<uint64_t> matrix{height, width, matrix_connected_components::kUnmarkedField<uint64_t>};
  for (int64_t row{0}; row < height; ++row) {
    for (int64_t column{0}; column < width; ++column) {
      if (input[asSizeT(row)][asSizeT(column)] == 'x') {
        matrix.get(row, column) = matrix_connected_components::kMarkedField<uint64_t>;
      }
    }
  }
  return matrix;
}

utils::containers::Matrix<uint64_t> makeExpectedMatrix(const std::vector<std::vector<uint64_t>> &input) {
  const auto height = static_cast<int64_t>(input.size());
  const auto width = static_cast<int64_t>(input.front().size());

  utils::containers::Matrix<uint64_t> matrix{height, width, matrix_connected_components::kUnmarkedField<uint64_t>};
  for (int64_t row{0}; row < height; ++row) {
    for (int64_t column{0}; column < width; ++column) {
      matrix.get(row, column) = input[asSizeT(row)][asSizeT(column)];
    }
  }
  return matrix;
}

static_assert(0 == matrix_connected_components::kUnmarkedField<uint64_t>);
static_assert(1 == matrix_connected_components::kMarkedField<uint64_t>);

TEST_CASE("SampleInput") {
  auto matrix = makeMatrix({
      {"x...."},
      {"x.xx."},
      {".x.x."},
      {".xxx."},
      {"....x"},
  });
  const auto expectedMatrix = makeExpectedMatrix({
      {2, 0, 0, 0, 0},
      {2, 0, 3, 3, 0},
      {0, 3, 0, 3, 0},
      {0, 3, 3, 3, 0},
      {0, 0, 0, 0, 5},
  });

  matrix_connected_components::labelConnectedCompnents(matrix);
  for (int64_t row{0}; row < matrix.height(); ++row) {
    for (int64_t column{0}; column < matrix.width(); ++column) {
      INFO("Row: " << row << ", column: " << column);
      CHECK(expectedMatrix.get(row, column) == matrix.get(row, column));
    }
  }
}
} // namespace matrix_connected_components::tests
