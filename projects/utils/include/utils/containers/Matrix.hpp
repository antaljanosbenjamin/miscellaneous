#pragma once

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace utils {

template <typename TValue>
class Matrix {
public:
  using ValueType = typename std::vector<TValue>::value_type;

  Matrix(int64_t height, int64_t width, TValue defaultValue = TValue{})
    : m_height(height)
    , m_width(width)
    , m_values(this->m_height * this->m_width, defaultValue) {
    if (this->m_height < 1) {
      throw std::invalid_argument{"The height of the matrix must be positive!"};
    }
    if (this->m_width < 1) {
      throw std::invalid_argument{"The width of the matrix must be positive!"};
    }
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
      this->m_height = other.m_height;
      this->m_width = other.m_width;
      this->m_values = other.m_values;
    }
    return *this;
  }

  ~Matrix() = default;

  template <typename TContainer>
  static Matrix<TValue> sameSizeAs(const TContainer &container) {
    return Matrix<TValue>{container.height(), container.width()};
  }

  TValue &get(const int64_t row, const int64_t column) {
    return this->m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  const TValue &get(const int64_t row, const int64_t column) const {
    return this->m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  TValue *tryGet(const int64_t row, const int64_t column) {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  const TValue *tryGet(const int64_t row, const int64_t column) const {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  TValue &getChecked(const int64_t row, const int64_t column) {
    if (const auto *ptr = this->tryGet(row, column); nullptr == ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  const TValue *getChecked(const int64_t row, const int64_t column) const {
    if (const auto *ptr = this->tryGet(row, column); nullptr == ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  [[nodiscard]] int64_t height() const noexcept {
    return this->m_height;
  }

  [[nodiscard]] int64_t width() const noexcept {
    return this->m_width;
  }

private:
  [[nodiscard]] int64_t getIndexFromRowAndColumn(const int64_t row, const int64_t column) const noexcept {
    return row * this->m_width + column;
  }

  [[nodiscard]] bool isValidRow(const int64_t row) const noexcept {
    return row < this->m_height;
  }

  [[nodiscard]] bool isValidColumn(const int64_t column) const noexcept {
    return column < this->m_width;
  }

  [[nodiscard]] bool areValidRowAndColumn(const int64_t row, const int64_t column) const noexcept {
    return this->isValidRow(row) && this->isValidColumn(column);
  }

  // Signed sizes https://www.open-std.org/JTC1/sc22/wg21/docs/papers/2019/p1428r0.pdf
  int64_t m_height;
  int64_t m_width;
  std::vector<TValue> m_values;
};
} // namespace utils