#pragma once

#include <concepts>
#include <stdexcept>
#include <vector>

namespace utils {

template <typename TValue>
class Matrix {
public:
  using ValueType = typename std::vector<TValue>::value_type;

  Matrix(size_t height, size_t width, TValue defaultValue = TValue{})
    : m_height(height)
    , m_width(width)
    , m_values(this->m_height * this->m_width, defaultValue) {
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

  TValue &get(const size_t row, const size_t column) {
    return this->m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  const TValue &get(const size_t row, const size_t column) const {
    return this->m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  TValue *tryGet(const size_t row, const size_t column) {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->m_values[this->getIndexFromRowAndColumn(row, column)];
    }
    return nullptr;
  }

  const TValue *tryGet(const size_t row, const size_t column) const {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->m_values[this->getIndexFromRowAndColumn(row, column)];
    }
    return nullptr;
  }

  TValue &getChecked(const size_t row, const size_t column) {
    if (this->areValidRowAndColumn(row, column)) {
      return this->m_values[this->getIndexFromRowAndColumn(row, column)];
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  const TValue *getChecked(const size_t row, const size_t column) const {

    if (this->areValidRowAndColumn(row, column)) {
      return this->m_values[this->getIndexFromRowAndColumn(row, column)];
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  [[nodiscard]] size_t height() const noexcept {
    return this->m_height;
  }

  [[nodiscard]] size_t width() const noexcept {
    return this->m_width;
  }

private:
  [[nodiscard]] size_t getIndexFromRowAndColumn(const size_t row, const size_t column) const noexcept {
    return row * this->m_width + column;
  }

  [[nodiscard]] bool isValidRow(const size_t row) const noexcept {
    return row < this->m_height;
  }

  [[nodiscard]] bool isValidColumn(const size_t column) const noexcept {
    return column < this->m_width;
  }

  [[nodiscard]] bool areValidRowAndColumn(const size_t row, const size_t column) const noexcept {
    return this->isValidRow(row) && this->isValidColumn(column);
  }

  size_t m_height;
  size_t m_width;
  std::vector<TValue> m_values;
};
} // namespace utils