#include <catch2/catch.hpp>

#include "utils/containers/Matrix.hpp"
#include "utils/containers/ValueTypeOf.hpp"

namespace utils::containers::tests {

static_assert(std::same_as<ValueTypeOf<Matrix<int64_t>>, int64_t>, "ValueTypeOf doesn't work with Matrix");
static_assert(std::same_as<ValueTypeOf<Matrix<uint64_t>>, uint64_t>, "ValueTypeOf doesn't work with Matrix");

template <typename TCheck, typename TValue = uint64_t>
void TestWithCopyCtor(const Matrix<TValue> &matrix, TCheck check) {
  INFO("CopyCtor");
  auto copiedMatrix = matrix;
  check(copiedMatrix);
}

template <typename TCheck, typename TValue = uint64_t>
void TestWithCopyAssignment(const Matrix<TValue> &matrix, TCheck check) {
  INFO("CopyAssignment");
  Matrix<TValue> copiedMatrix{0, 0};
  copiedMatrix = matrix;
  check(copiedMatrix);
}

// Requires copy ctor
template <typename TCheck, typename TValue = uint64_t>
void TestWithMoveCtor(const Matrix<TValue> &matrix, TCheck check) {
  INFO("MoveCtor");
  auto copiedMatrix = matrix;
  auto movedMatrix = std::move(copiedMatrix);
  check(movedMatrix);
  CHECK(copiedMatrix.width() == 0);
  CHECK(copiedMatrix.height() == 0);
}

// Requires copy ctor
template <typename TCheck, typename TValue = uint64_t>
void TestWithMoveAssignment(const Matrix<TValue> &matrix, TCheck check) {
  INFO("MoveAssignment");
  auto copiedMatrix = matrix;
  Matrix<TValue> movedMatrix{0, 0};
  movedMatrix = std::move(copiedMatrix);
  check(movedMatrix);
  CHECK(copiedMatrix.width() == 0);
  CHECK(copiedMatrix.height() == 0);
}

template <typename TMatrix, typename TCheck>
void TestWithConst(const TMatrix &matrix, TCheck check) {
  check(matrix);
}

template <typename TCheck, typename TValue = uint64_t>
void TestScenarios(Matrix<TValue> &matrix, TCheck check) {
  TestWithCopyCtor(matrix, check);
  TestWithCopyAssignment(matrix, check);
  TestWithMoveCtor(matrix, check);
  TestWithMoveAssignment(matrix, check);
  TestWithConst(matrix, check);
  check(matrix);
}

template <typename TMatrix>
void CheckValue(TMatrix &matrix, const int64_t row, const int64_t column, const ValueTypeOf<TMatrix> expectedValue) {
  auto check = [row, column, expectedValue](auto &matrix) {
    CHECK(expectedValue == matrix.get(row, column));
    CHECK(expectedValue == matrix.getChecked(row, column));
    const auto *ptr = matrix.tryGet(row, column);
    CHECK(nullptr != ptr);
    CHECK(expectedValue == *ptr);
  };
  check(matrix);
}

TEST_CASE("EmptyMatrix") {
  Matrix<uint64_t> matrix{0, 0};

  const auto check = [](auto &matrix) {
    CHECK(matrix.width() == 0);
    CHECK(matrix.height() == 0);
    CHECK_THROWS(matrix.getChecked(0, 0));
    CHECK(nullptr == matrix.tryGet(0, 0));
  };
  TestScenarios(matrix, check);
  check(matrix);
}

TEST_CASE("DefaultValue") {
  constexpr int64_t kWidth = 3;
  constexpr int64_t kHeight = 5;
  constexpr uint64_t kDefaultValue = 42;
  Matrix<uint64_t> matrix{kHeight, kWidth, kDefaultValue};

  const auto check = [](auto &matrix) {
    for (int64_t row{0}; row < kHeight; ++row) {
      for (int64_t column{0}; column < kWidth; ++column) {
        CheckValue(matrix, row, column, kDefaultValue);
      }
    }
  };
  TestScenarios(matrix, check);
}

TEST_CASE("SetValue") {
  constexpr int64_t kWidth = 3;
  constexpr int64_t kHeight = 5;
  constexpr uint64_t kDefaultValue = 42;
  Matrix<uint64_t> matrix{kHeight, kWidth, kDefaultValue};
  for (int64_t row{0}; row < kHeight; ++row) {
    for (int64_t column{0}; column < kWidth; ++column) {
      switch (row * column % 3) {
      case 0:
        matrix.get(row, column) = row * column;
        break;
      case 1:
        matrix.getChecked(row, column) = row * column;
        break;
      case 2:
        *matrix.tryGet(row, column) = row * column;
        break;
      }
    }
  }

  const auto check = [](auto &matrix) {
    for (int64_t row{0}; row < kHeight; ++row) {
      for (int64_t column{0}; column < kWidth; ++column) {
        CheckValue(matrix, row, column, row * column);
      }
    }
  };
  TestScenarios(matrix, check);
}

TEST_CASE("InvalidIndices") {
  constexpr int64_t kWidth = 3;
  constexpr int64_t kHeight = 2;
  constexpr uint64_t kDefaultValue = 42;
  Matrix<uint64_t> matrix{kHeight, kWidth, kDefaultValue};
  for (int64_t row{0}; row < kHeight; ++row) {
    for (int64_t column{0}; column < kWidth; ++column) {
      switch (row * column % 3) {
      case 0:
        matrix.get(row, column) = row * column;
        break;
      case 1:
        matrix.getChecked(row, column) = row * column;
        break;
      case 2:
        *matrix.tryGet(row, column) = row * column;
        break;
      }
    }
  }

  const auto check = [](auto &matrix) {
    for (int64_t column{0}; column < kWidth; ++column) {
      CHECK_THROWS(matrix.getChecked(-1, column));
      CHECK(nullptr == matrix.tryGet(-1, column));
      CHECK_THROWS(matrix.getChecked(kHeight, column));
      CHECK(nullptr == matrix.tryGet(kHeight, column));
    }
    for (int64_t row{0}; row < kHeight; ++row) {
      CHECK_THROWS(matrix.getChecked(row, -1));
      CHECK(nullptr == matrix.tryGet(row, -1));
      CHECK_THROWS(matrix.getChecked(row, kWidth));
      CHECK(nullptr == matrix.tryGet(row, kWidth));
    }
  };
  TestScenarios(matrix, check);
}

} // namespace utils::containers::tests
