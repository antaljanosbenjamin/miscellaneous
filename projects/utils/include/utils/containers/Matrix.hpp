#pragma once

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace utils::containers {

template <typename TValue>
class Matrix {
public:
  using ValueType = typename std::vector<TValue>::value_type;

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

  [[nodiscard]] TValue &get(const int64_t row, const int64_t column) {
    return m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  [[nodiscard]] const TValue &get(const int64_t row, const int64_t column) const {
    return m_values[this->getIndexFromRowAndColumn(row, column)];
  }

  [[nodiscard]] TValue *tryGet(const int64_t row, const int64_t column) {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  [[nodiscard]] const TValue *tryGet(const int64_t row, const int64_t column) const {
    if (this->areValidRowAndColumn(row, column)) {
      return &this->get(row, column);
    }
    return nullptr;
  }

  [[nodiscard]] TValue &getChecked(const int64_t row, const int64_t column) {
    if (auto *ptr = this->tryGet(row, column); nullptr != ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  [[nodiscard]] const TValue &getChecked(const int64_t row, const int64_t column) const {
    if (const auto *ptr = this->tryGet(row, column); nullptr != ptr) {
      return *ptr;
    }
    throw std::out_of_range{"Invalid row or column"};
  }

  [[nodiscard]] int64_t height() const noexcept {
    return m_height;
  }

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
