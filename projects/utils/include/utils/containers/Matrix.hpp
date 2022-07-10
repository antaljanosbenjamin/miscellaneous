#pragma once

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace utils::containers {

// A container represents a 2D matrix.
template <typename TValue>
class Matrix {
public:
  using ValueType = typename std::vector<TValue>::value_type;

  // The parameters are the following:
  // - `height`: the height of the 2D matrix
  // - `width`: the width of the 2D matrix
  // - `defaultValue`: the matrix will be filled with it after construction
  // Throws `std::invalid_argument` if any of the sizes is less than zero.
  Matrix(int64_t height, int64_t width, TValue defaultValue = TValue{})
    : m_height{height}
    , m_width{width} {
    if (m_height < 0) {
      throw std::invalid_argument{"The height of the matrix cannot be negative!"};
    }
    if (m_width < 0) {
      throw std::invalid_argument{"The width of the matrix cannot be negative!"};
    }
    // The creation of the vector has to be done after checking the size, otherwise the constructor of the vector might
    // throw an exception.
    m_values = std::vector<ValueType>(m_height * m_width, defaultValue);
  }

  Matrix(const Matrix &) = default;
  Matrix &operator=(const Matrix &) = default;

  Matrix(Matrix &&other) noexcept
    : m_height(other.m_height)
    , m_width(other.m_width)
    , m_values(std::move(other.m_values)) {
    other.m_height = 0;
    other.m_width = 0;
  }

  Matrix &operator=(Matrix &&other) noexcept {
    if (this != &other) {
      m_height = other.m_height;
      m_width = other.m_width;
      m_values = std::move(other.m_values);
      other.m_height = 0;
      other.m_width = 0;
    }
    return *this;
  }

  ~Matrix() = default;

  // Returns a reference to the item specified by its row and column. The behavior is undefined if `row` or `column` is
  // not a valid index. Valid indices are greater or equal than zero and less than the corresponding size of the matrix.
  // Complexity: constant
  [[nodiscard]] TValue &get(const int64_t row, const int64_t column) {
    return m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  // Returns a constant reference to the item specified by its row and column. The behavior is undefined if `row` or
  // `column` is not a valid index. Valid indices are greater or equal than zero and less than the corresponding size of
  // the matrix.
  // Complexity: constant
  [[nodiscard]] const TValue &get(const int64_t row, const int64_t column) const {
    return m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  // Returns a pointer to the item specified by its row and column, or `nullptr` if `row` or `column` is not a valid
  // index. Valid indices are greater or equal than zero and less than the corresponding size of the matrix.
  // Complexity: constant
  [[nodiscard]] TValue *tryGet(const int64_t row, const int64_t column) {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  // Returns a constant pointer to the item specified by its row and column, or `nullptr` if `row` or `column` is not a
  // valid index. Valid indices are greater or equal than zero and less than the corresponding size of the matrix.
  // Complexity: constant
  [[nodiscard]] const TValue *tryGet(const int64_t row, const int64_t column) const {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  // Returns a reference to the item specified by its row and column, or throws a `std::out_of_range` exception if `row`
  // or `column` is not a valid index. Valid indices are greater or equal than zero and less than the corresponding size
  // of the matrix.
  // Complexity: constant
  [[nodiscard]] TValue &getChecked(const int64_t row, const int64_t column) {
    if (auto *ptr = this->tryGet(row, column); nullptr != ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  // Returns a constant reference to the item specified by its row and column, or throws a `std::out_of_range` exception
  // if `row` or `column` is not a valid index. Valid indices are greater or equal than zero and less than the
  // corresponding size of the matrix.
  // Complexity: constant
  [[nodiscard]] const TValue &getChecked(const int64_t row, const int64_t column) const {
    if (const auto *ptr = this->tryGet(row, column); nullptr != ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  // Returns the height of the matrix.
  // Complexity: constant
  [[nodiscard]] int64_t height() const noexcept {
    return m_height;
  }

  // Returns the width of the matrix.
  // Complexity: constant
  [[nodiscard]] int64_t width() const noexcept {
    return m_width;
  }

private:
  [[nodiscard]] int64_t getIndexFromRowAndColumn(const int64_t row, const int64_t column) const noexcept {
    return row * m_width + column;
  }

  [[nodiscard]] bool isValidRow(const int64_t row) const noexcept {
    return 0 <= row && row < m_height;
  }

  [[nodiscard]] bool isValidColumn(const int64_t column) const noexcept {
    return 0 <= column && column < m_width;
  }

  [[nodiscard]] bool areValidRowAndColumn(const int64_t row, const int64_t column) const noexcept {
    return this->isValidRow(row) && this->isValidColumn(column);
  }

  // Signed sizes https://www.open-std.org/JTC1/sc22/wg21/docs/papers/2019/p1428r0.pdf
  int64_t m_height;
  int64_t m_width;
  std::vector<TValue> m_values;
};
} // namespace utils::containers
