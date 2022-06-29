#include "Algorithm.hpp"

#include <cstdint>

#include "MatrixSlice.hpp"

namespace matrix_connected_components {

void doMagic(const utils::Matrix<uint8_t> &matrix) {
  auto labelledMatrix = utils::Matrix<uint64_t>::sameSizeAs(matrix);

  if (labelledMatrix.width() != matrix.width()) {
    throw 'w';
  }
  if (labelledMatrix.height() != matrix.height()) {
    throw 'h';
  }
  auto labelledMatrixSlice = MatrixSlice{labelledMatrix, 0, 0, 2, 4};
  auto labelledMatrixSliceSlice = MatrixSlice{labelledMatrixSlice, 1, 1, 1, 2};
}

} // namespace matrix_connected_components