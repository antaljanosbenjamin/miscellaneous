#include "Algorithm.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <unordered_set>

#include "MatrixSlice.hpp"
#include "utils/DisjointSets.hpp"

namespace matrix_connected_components {

void doMagic(const utils::Matrix<uint8_t> &matrix) {

  using LabelType = int64_t;
  static constexpr LabelType kUnmarked = 0;
  static constexpr LabelType kMarked = 1;
  static constexpr LabelType kFirstLabel = kMarked + 1;

  utils::Matrix<LabelType> labelledMatrix{matrix.height() + 2, matrix.width() + 2, kUnmarked};

  for (auto row = 0L; row < matrix.height(); ++row) {
    for (auto column = 0L; column < matrix.width(); ++column) {
      if (matrix.get(row, column) != 0) {
        labelledMatrix.get(row + 1, column + 1) = kMarked;
      }
    }
  }

  const auto firstRow = 1L;
  const auto firstColumn = 1L;
  const auto lastRow = labelledMatrix.height() - 2;
  const auto lastColumn = labelledMatrix.width() - 2;

  utils::DisjointSets labelUnions;
  labelUnions.add(kUnmarked);
  labelUnions.add(kMarked);
  [[maybe_unused]] auto print = [&]() {
    for (auto row = 0U; row < labelledMatrix.height(); ++row) {
      for (auto column = 0U; column < labelledMatrix.width(); ++column) {
        std::cout << static_cast<int>(*labelUnions.find(labelledMatrix.get(row, column))) << ' ';
      }
      std::cout << std::endl;
    }
  };
  auto currentLabel = kFirstLabel;
  for (auto row = firstRow; row <= lastRow; ++row) {
    for (auto column = firstColumn; column <= lastColumn; ++column) {
      auto &label = labelledMatrix.get(row, column);
      if (label == kUnmarked) {
        continue;
      }
      int64_t smallerLabel{kUnmarked};
      int64_t greaterLabel{kUnmarked};
      if (row > firstRow) {
        smallerLabel = labelledMatrix.get(row - 1, column);
      }
      if (column > firstColumn) {
        greaterLabel = labelledMatrix.get(row, column - 1);
      }
      if (greaterLabel < smallerLabel) {
        std::swap(smallerLabel, greaterLabel);
      }

      if (smallerLabel == kUnmarked) {
        // we don't have any collision for sure
        if (greaterLabel == kUnmarked) {
          // We need a new label
          label = currentLabel++;

          labelUnions.add(label);

        } else {
          label = greaterLabel;
        }
      } else {
        // we might have a collision
        label = smallerLabel;
        if (smallerLabel != greaterLabel) {

          labelUnions.merge(smallerLabel, greaterLabel);
        }
      }
      // print();
      // std::cout << "--------------------------\n";
    }
  }
}

} // namespace matrix_connected_components