#pragma once

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include "matrix_connected_components/MatrixUtils.hpp"
#include "utils/NotNull.hpp"
#include "utils/containers/Matrix.hpp"
#include "utils/containers/ValueTypeOf.hpp"

namespace matrix_connected_components {

template <IsNumericalMatrixLike TMatrix>
class MatrixSlice {
public:
  using ValueType = utils::containers::ValueTypeOf<TMatrix>;

  MatrixSlice(TMatrix &matrix, int64_t height_offset, int64_t width_offset, int64_t height, int64_t width)
    : m_height_offset{height_offset}
    , m_width_offset{width_offset}
    , m_height{height}
    , m_width{width}
    , m_matrix{&matrix} {
    // TODO(antaljanosbenjamin): improve checks
    if (this->m_height_offset + this->m_height > this->m_matrix->height()) {
      throw std::invalid_argument{"Invalid height ot height offset"};
    }
    if (this->m_width_offset + this->m_width > this->m_matrix->width()) {
      throw std::invalid_argument{"Invalid height ot height offset"};
    }
  }

  MatrixSlice(const MatrixSlice &) noexcept = default;
  MatrixSlice &operator=(const MatrixSlice &) noexcept = default;
  MatrixSlice(MatrixSlice &&) noexcept = default;
  MatrixSlice &operator=(MatrixSlice &&) noexcept = default;

  ~MatrixSlice() = default;

  [[nodiscard]] int64_t heightOffset() const noexcept {
    return this->m_height_offset;
  }
  [[nodiscard]] int64_t widthOffset() const noexcept {
    return this->m_width_offset;
  }
  [[nodiscard]] int64_t height() const noexcept {
    return this->m_height;
  }
  [[nodiscard]] int64_t width() const noexcept {
    return this->m_width;
  }

  [[nodiscard]] ValueType &get(const int64_t row, const int64_t column) {
    return this->m_matrix->get(this->m_height_offset + row, this->m_width_offset + column);
  }

  [[nodiscard]] const ValueType &get(const int64_t row, const int64_t column) const {
    return this->m_matrix->get(this->m_height_offset + row, this->m_width_offset + column);
  }

private:
  int64_t m_height_offset{0};
  int64_t m_width_offset{0};
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

} // namespace matrix_connected_components
