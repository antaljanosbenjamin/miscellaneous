#pragma once

#include <concepts>
#include <cstdint>
#include <ctime>
#include <limits>

#include "matrix_connected_components/MatrixSlice.hpp"
#include "matrix_connected_components/MatrixUtils.hpp"
#include "utils/Concepts.hpp"
#include "utils/containers/DisjointSet.hpp"
#include "utils/containers/Matrix.hpp"
#include "utils/containers/ValueTypeOf.hpp"

namespace matrix_connected_components {

template <utils::NumericIntegral TValue>
static constexpr TValue kUnmarkedField = std::numeric_limits<TValue>::min();

template <utils::NumericIntegral TValue>
static constexpr TValue kMarkedField = kUnmarkedField<TValue> + 1;

template <typename T>
using ValueTypeOf = utils::containers::ValueTypeOf<T>;

template <typename T>
using DisjointSet = utils::containers::DisjointSet<T>;

template <IsNumericalMatrixLike TMatrix>
DisjointSet<ValueTypeOf<TMatrix>> assignInitialLabels(TMatrix &matrix);

template <IsNumericalMatrixLike TMatrix>
void relabelMatrix(TMatrix &matrix, DisjointSet<ValueTypeOf<TMatrix>> &labelSets);

// # General description of the algorithm
//
// Labelling the connected components in a matrix works in two phases:
// 1. First, assign an initial label to every marked field of the matrix and register the conflicts when two different
// labels are on neighboring fields.
// 2. After the conflicts are resolved, all of the initial labels can be replaced to final labels, thus assigning the
// final label for all of fields. After this step, all the fields in a connected component has the same label and no two
// components will have the same label.
//
// # First phase
//
// The next free label value is always maintained during the algorithm, but it's actual value is not that relevant. In
// the first phase, all the fields are marched in a raster pattern (from top to bottom line by line, then in a single
// line from the left to the right). To determine the initial label for a field, two of neighbors, the left and the top
// are checked:
// 1. If only one of them is labelled, then the current field will get the same label.
// 2. If both of them are labelled, then the current field will get the label with the smallest value.
// 3. If none of them are labelled, then the current field will get the next free label (and therefore the value of the
//    next free label is incremented).
// In case of #3, we have to also deal with the conflict, because that case means two differently labelled area are next
// to each other. For this, we are maintaining the a DisjointSet (UnionFind) data structure which is optimized for
// merging disjoint sets of integers. Every time we detect a conflict, the sets of the two different labels has to be
// merged. This way all of the labels occurring in a single connected component will be merged into a single set in the
// DisjointSet data structure.
//
// # Second phase
//
// In the second phase we just iterate over the matrix and update the label of each field to the lowest label of their
// label set using the DisjointSet data structure built up in first phase.
//
// Alternatively if we only care about the number if connected components, it is enough to get the number of disjoint
// sets from the same data structure.

// This function takes a matrix-like object by value and returns the same type of matrix-like object with all of the
// marked fields labelled with a number that is unique to component they are part of.
// The fields of the input matrix can have 2 values:
// 1. `kUnmarkedField<ValueType>` means the field is not marked, thus it cannot be part of any of the components. It
//    won't be labelled in the algorithm.
// 2. `kMarkedField<ValueType>` means the field is marked, so it has to be part one of components. It will be labelled
//    as part of the algorithm.
// If an input object contains any other value, then the behavior of the algorithm is undefined.
template <IsNumericalMatrixLike TMatrix>
[[nodiscard]] TMatrix labelConnectedComponents(TMatrix matrix) {
  auto labelSets = assignInitialLabels(matrix);
  relabelMatrix(matrix, labelSets);
  return matrix;
}

// This function takes a matrix-like object by value and returns the number of connected components in the matrix. The
// input object has to adhere to the same restrictions as for the `labelConnectedComponents` function.
template <IsNumericalMatrixLike TMatrix>
[[nodiscard]] int64_t countConnectedComponents(TMatrix matrix) {
  return assignInitialLabels(matrix).numberOfDisjointSets();
}

// This function takes a matrix-like object by reference and assigns the initial labels to the marked fields in the
// matrix and returns the sets of labels in a DisjointSet data structure.
template <IsNumericalMatrixLike TMatrix>
DisjointSet<ValueTypeOf<TMatrix>> assignInitialLabels(TMatrix &matrix) {

  using ValueType = ValueTypeOf<TMatrix>;
  using LabelType = ValueType;

  DisjointSet<LabelType> labelSets;
  static constexpr ValueType kUnmarked = kUnmarkedField<ValueType>;
  static constexpr ValueType kMarked = kMarkedField<ValueType>;
  static constexpr ValueType kFirstLabel = kMarked + 1;

  auto currentLabel = kFirstLabel;
  const auto width = matrix.width();
  const auto height = matrix.height();
  for (auto row{0}; row < height; ++row) {
    for (auto column{0}; column < width; ++column) {
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
        // we don't have any conflict for sure
        if (greaterLabel == kUnmarked) {
          // We need a new label
          label = currentLabel++;

          labelSets.add(label);

        } else {
          label = greaterLabel;
        }
      } else {
        // we might have a conflict
        label = smallerLabel;
        if (smallerLabel != greaterLabel) {
          labelSets.merge(smallerLabel, greaterLabel);
        }
      }
    }
  }
  return labelSets;
}

// This function takes a matrix-like object by reference and the disjoint sets of labels. Iterates over all of marked
// fields and update the label of each field to the lowest label of their label set. If the input matrix contains any
// field that is not unmarked or it doesn't belong to any of the label sets, then the result of the algorithm is
// undefined.
template <IsNumericalMatrixLike TMatrix>
void relabelMatrix(TMatrix &matrix, DisjointSet<ValueTypeOf<TMatrix>> &labelSets) {
  using ValueType = ValueTypeOf<TMatrix>;
  static constexpr ValueType kUnmarked = kUnmarkedField<ValueType>;
  const auto width = matrix.width();
  const auto height = matrix.height();
  for (auto row{0}; row < height; ++row) {
    for (auto column{0}; column < width; ++column) {
      auto &label = matrix.get(row, column);
      if (label != kUnmarked) {
        label = *labelSets.find(label);
      }
    }
  }
}
} // namespace matrix_connected_components
