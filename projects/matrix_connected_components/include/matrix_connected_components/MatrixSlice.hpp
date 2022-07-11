#pragma once

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include "matrix_connected_components/MatrixUtils.hpp"
#include "utils/NotNull.hpp"
#include "utils/containers/Matrix.hpp"
#include "utils/containers/ValueTypeOf.hpp"

namespace matrix_connected_components {

// A simple wrapper class that can be used to restrict the access to a contiguous, rectangular-shaped part of a
// matrix-like object.
template <IsNumericalMatrixLike TMatrix>
class MatrixSlice {
public:
  using ValueType = utils::containers::ValueTypeOf<TMatrix>;

  // The parameters are the following:
  // - `matrix`: the matrix-like object to be wrapped
  // - `heightOffset`: the index of the first line that should be accessible through this wrapper
  // - `widthOffset`: the index of the first column that should be accessible through this wrapper
  // - `height`: the number of row that should accessible through this wrapper starting from the `heightOffset` row
  // - `width`: the number of columns that should accessible through this wrapper starting from the `widthOffset` column
  // Throws `std::invalid_argument` if:
  // - any of the four mentioned values are less then 0
  // - if `height + heightOffset > matrix.height()` or `width + widthOffset > matrix.width()` is true
  MatrixSlice(TMatrix &matrix, int64_t heightOffset, int64_t widthOffset, int64_t height, int64_t width)
    : m_heightOffset{heightOffset}
    , m_widthOffset{widthOffset}
    , m_height{height}
    , m_width{width}
    , m_matrix{&matrix} {
    if (m_height < 0) {
      throw std::invalid_argument{"The height of the slice cannot be negative!"};
    }
    if (m_width < 0) {
      throw std::invalid_argument{"The width of the slice cannot be negative!"};
    }
    if (m_heightOffset < 0) {
      throw std::invalid_argument{"The height offset of the slice cannot be negative!"};
    }
    if (m_widthOffset < 0) {
      throw std::invalid_argument{"The width offset of the slice cannot be negative!"};
    }
    if (m_heightOffset + m_height > m_matrix->height()) {
      throw std::invalid_argument{
          "The height plus the height offset is greater than the height of the underlying matrix!"};
    }
    if (m_widthOffset + m_width > m_matrix->width()) {
      throw std::invalid_argument{
          "The width plus the width offset is greater than the width of the underlying matrix!"};
    }
  }

  MatrixSlice(const MatrixSlice &) noexcept = default;
  MatrixSlice &operator=(const MatrixSlice &) noexcept = default;
  MatrixSlice(MatrixSlice &&) noexcept = default;
  MatrixSlice &operator=(MatrixSlice &&) noexcept = default;

  ~MatrixSlice() = default;

  // Returns the height offset of the slice.
  // Complexity: constant
  [[nodiscard]] int64_t heightOffset() const noexcept {
    return m_heightOffset;
  }

  // Returns the width offset of the slice.
  // Complexity: constant
  [[nodiscard]] int64_t widthOffset() const noexcept {
    return m_widthOffset;
  }

  // Returns the height of the slice.
  // Complexity: constant
  [[nodiscard]] int64_t height() const noexcept {
    return m_height;
  }

  // Returns the width of the slice.
  // Complexity: constant
  [[nodiscard]] int64_t width() const noexcept {
    return m_width;
  }

  // Returns a reference to the item specified by its row and column. The behavior is undefined if `row` or `column` is
  // not a valid index. Valid indices are greater or equal than zero and less than the corresponding size of the slice.
  // Complexity: constant
  [[nodiscard]] ValueType &get(const int64_t row, const int64_t column) {
    return m_matrix->get(m_heightOffset + row, m_widthOffset + column);
  }

  // Returns a constant reference to the item specified by its row and column. The behavior is undefined if `row` or
  // `column` is not a valid index. Valid indices are greater or equal than zero and less than the corresponding size of
  // the slice.
  // Complexity: constant
  [[nodiscard]] const ValueType &get(const int64_t row, const int64_t column) const {
    return m_matrix->get(m_heightOffset + row, m_widthOffset + column);
  }

private:
  int64_t m_heightOffset{0};
  int64_t m_widthOffset{0};
  int64_t m_height{0};
  int64_t m_width{0};
  utils::NotNull<TMatrix *> m_matrix;
};

template <typename TMatrix>
MatrixSlice(TMatrix, int64_t, int64_t, int64_t, int64_t) -> MatrixSlice<TMatrix>;

static_assert(IsNumericalMatrixLike<MatrixSlice<utils::containers::Matrix<uint64_t>>>,
              "MatrixSlice has to satisfy IsNumericalMatrixLike, please check the requirements");
static_assert(IsNumericalMatrixLike<MatrixSlice<utils::containers::Matrix<int64_t>>>,
              "MatrixSlice has to satisfy IsNumericalMatrixLike, please check the requirements");
static_assert(std::is_trivially_copyable_v<MatrixSlice<utils::containers::Matrix<int64_t>>>,
              "MatrixSlice meant to be trivially copyable!");

} // namespace matrix_connected_components
