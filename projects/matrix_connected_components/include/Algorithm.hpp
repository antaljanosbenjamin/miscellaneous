#pragma once

#include <concepts>
#include <cstdint>
#include <limits>

#include "utils/Concepts.hpp"
#include "utils/DisjointSets.hpp"
#include "utils/Matrix.hpp"

namespace matrix_connected_components {

template <utils::NumericIntegral TValue>
static constexpr TValue kUnmarkedField = std::numeric_limits<TValue>::min();

template <utils::NumericIntegral TValue>
static constexpr TValue kMarkedField = kUnmarkedField<TValue> + 1;

template <utils::NumericIntegral TValue>
void doMagic(utils::Matrix<TValue> &matrix) {

  using LabelType = TValue;
  utils::DisjointSets<LabelType> labelUnions;
  static constexpr TValue kUnmarked = kUnmarkedField<TValue>;
  static constexpr TValue kMarked = kMarkedField<TValue>;
  static constexpr TValue kFirstLabel = kMarked + 1;

  auto currentLabel = kFirstLabel;
  const auto width = matrix.width();
  const auto height = matrix.height();
  for (auto row = 0; row < height; ++row) {
    for (auto column = 0; column < width; ++column) {
      auto &label = matrix.get(row, column);
      if (label == kUnmarked) {
        continue;
      }
      LabelType smallerLabel{kUnmarked};
      LabelType greaterLabel{kUnmarked};
      if (row > 0) {
        smallerLabel = matrix.get(row - 1, column);
      }
      if (column > 0) {
        greaterLabel = matrix.get(row, column - 1);
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
    }
  }
  for (auto row = 0; row < height; ++row) {
    for (auto column = 0; column < width; ++column) {
      auto &label = matrix.get(row, column);
      if (label != kUnmarked) {
        label = *labelUnions.find(label);
      }
    }
  }

  std::cout << "Size: " << labelUnions.size() << std::endl;
}

} // namespace matrix_connected_components
