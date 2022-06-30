#pragma once

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include "utils/Matrix.hpp"
#include "utils/NotNull.hpp"
#include "utils/PropagateConst.hpp"

namespace matrix_connected_components {

template <typename TMatrix, typename TMatrix::ValueType kDefaultValue = typename TMatrix::ValueType{}>
class MatrixSlice {

public:
  using ValueType = typename TMatrix::ValueType;

  MatrixSlice(TMatrix &matrix, int64_t height_offset, int64_t width_offset, int64_t height, int64_t width)
    : m_height_offset{height_offset}
    , m_width_offset{width_offset}
    , m_height{height}
    , m_width{width}
    , m_matrix{&matrix} {
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

private:
  int64_t m_height_offset{0};
  int64_t m_width_offset{0};
  int64_t m_height{0};
  int64_t m_width{0};
  utils::NotNull<TMatrix *> m_matrix;
};

template <typename TMatrix, typename TMatrix::ValueType kDefaultValue = typename TMatrix::ValueType{}>
MatrixSlice(TMatrix, int64_t, int64_t, int64_t, int64_t) -> MatrixSlice<TMatrix>;

static_assert(std::is_nothrow_move_constructible_v<MatrixSlice<utils::Matrix<int>>>);

} // namespace matrix_connected_components