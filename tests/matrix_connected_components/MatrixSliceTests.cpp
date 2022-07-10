#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "matrix_connected_components/Algorithm.hpp"
#include "utils/containers/Matrix.hpp"

namespace matrix_connected_components::tests {

using TestMatrix = utils::containers::Matrix<uint64_t>;

void checkSliceAndMatrix(MatrixSlice<TestMatrix> &slice, const TestMatrix &matrix) {
  const auto checkSlice = [&matrix](auto &slice) {
    REQUIRE(matrix.height() == slice.height());
    REQUIRE(matrix.width() == slice.width());
    REQUIRE(0 == slice.heightOffset());
    REQUIRE(0 == slice.widthOffset());
    for (int64_t row{0}; row < matrix.height(); ++row) {
      for (int64_t column{0}; column < matrix.width(); ++column) {
        CHECK(matrix.get(row, column) == slice.get(row, column));
      }
    }
  };
  checkSlice(slice);
  const auto &constSlice = slice;
  checkSlice(constSlice);
}

void checkPartialSliceAndMatrix(MatrixSlice<TestMatrix> &slice, const TestMatrix &matrix) {
  const auto checkSlice = [&matrix](auto &slice) {
    REQUIRE(matrix.height() >= slice.height() + slice.heightOffset());
    REQUIRE(matrix.width() >= slice.width() + slice.widthOffset());
    const auto heightOffset = slice.heightOffset();
    const auto widthOffset = slice.widthOffset();
    for (int64_t row{0}; row < slice.height(); ++row) {
      for (int64_t column{0}; column < slice.width(); ++column) {
        CHECK(matrix.get(heightOffset + row, widthOffset + column) == slice.get(row, column));
      }
    }
  };
  checkSlice(slice);
  const auto &constSlice = slice;
  checkSlice(constSlice);
}

TEST_CASE("ValidationsInCtor") {
  static constexpr int64_t kHeight{5};
  static constexpr int64_t kWidth{3};
  TestMatrix matrix{kHeight, kWidth};
  CHECK_THROWS_MATCHES((MatrixSlice{matrix, -1, 0, 0, 0}), std::invalid_argument,
                       Catch::Matchers::Message("The height offset of the slice cannot be negative!"));
  CHECK_THROWS_MATCHES((MatrixSlice{matrix, 0, -1, 0, 0}), std::invalid_argument,
                       Catch::Matchers::Message("The width offset of the slice cannot be negative!"));
  CHECK_THROWS_MATCHES((MatrixSlice{matrix, 0, 0, -1, 0}), std::invalid_argument,
                       Catch::Matchers::Message("The height of the slice cannot be negative!"));
  CHECK_THROWS_MATCHES((MatrixSlice{matrix, 0, 0, 0, -1}), std::invalid_argument,
                       Catch::Matchers::Message("The width of the slice cannot be negative!"));
  CHECK_THROWS_MATCHES((MatrixSlice{matrix, kHeight - 1, 0, 2, 0}), std::invalid_argument,
                       Catch::Matchers::Message(
                           "The height plus the height offset is greater than the height of the underlying matrix!"));
  CHECK_THROWS_MATCHES(
      (MatrixSlice{matrix, 0, 2, 0, kWidth - 1}), std::invalid_argument,
      Catch::Matchers::Message("The width plus the width offset is greater than the width of the underlying matrix!"));
  CHECK_NOTHROW(MatrixSlice(matrix, 0, 0, 0, 0));

  for (int64_t heightOffset = 0; heightOffset < kHeight; ++heightOffset) {
    for (int64_t widthOffset = 0; widthOffset < kWidth; ++widthOffset) {
      INFO("Height offset: " << heightOffset << ", width offset: " << widthOffset);
      CHECK_NOTHROW(MatrixSlice(matrix, heightOffset, widthOffset, 0, 0));
      CHECK_NOTHROW(MatrixSlice(matrix, heightOffset, widthOffset, kHeight - heightOffset, kWidth - widthOffset));
      CHECK_NOTHROW(MatrixSlice(matrix, kHeight - heightOffset, kWidth - widthOffset, heightOffset, widthOffset));
    }
  }
}

TEST_CASE("SizeTest") {
  static constexpr int64_t kHeight{3};
  static constexpr int64_t kWidth{7};
  TestMatrix matrix{kHeight, kWidth};

  const auto checkSlice = [](TestMatrix &matrix, const int64_t expectedHeightOffset, const int64_t expectedWidthOffset,
                             const int64_t expectedHeight, const int64_t expectedWidth) {
    INFO("expectedHeightOffset: " << expectedHeightOffset << ", expectedWidthOffset: " << expectedWidthOffset
                                  << "expectedHeight: " << expectedHeight << ", expectedWidth: " << expectedWidth);
    MatrixSlice slice(matrix, expectedHeightOffset, expectedWidthOffset, expectedHeight, expectedWidth);
    CHECK(expectedHeightOffset == slice.heightOffset());
    CHECK(expectedWidthOffset == slice.widthOffset());
    CHECK(expectedHeight == slice.height());
    CHECK(expectedWidth == slice.width());
  };
  for (int64_t heightOffset = 0; heightOffset < kHeight; ++heightOffset) {
    for (int64_t widthOffset = 0; widthOffset < kWidth; ++widthOffset) {
      INFO("Height offset: " << heightOffset << ", width offset: " << widthOffset);
      checkSlice(matrix, heightOffset, widthOffset, 1, 1);
      checkSlice(matrix, heightOffset, widthOffset, kHeight - heightOffset, kWidth - widthOffset);
      checkSlice(matrix, kHeight - heightOffset, kWidth - widthOffset, heightOffset, widthOffset);
    }
  }
}

TEST_CASE("AccessToFullMatrix") {
  static constexpr int64_t kHeight{3};
  static constexpr int64_t kWidth{4};
  TestMatrix matrix{kHeight, kWidth};

  for (int64_t row{0}; row < kHeight; ++row) {
    for (int64_t column{0}; column < kWidth; ++column) {
      matrix.get(row, column) = row * kWidth + column;
    }
  }
  MatrixSlice slice{matrix, 0, 0, kHeight, kWidth};
  checkSliceAndMatrix(slice, matrix);
}

TEST_CASE("PartialAccess") {
  static constexpr int64_t kHeight{3};
  static constexpr int64_t kWidth{4};
  TestMatrix matrix{kHeight, kWidth};

  for (int64_t row{0}; row < kHeight; ++row) {
    for (int64_t column{0}; column < kWidth; ++column) {
      matrix.get(row, column) = row * kWidth + column;
    }
  }

  for (int64_t sliceHeight{1}; sliceHeight <= kHeight; ++sliceHeight) {
    for (int64_t sliceWidth{1}; sliceWidth <= kWidth; ++sliceWidth) {
      for (int64_t sliceHeightOffset{0}; sliceHeightOffset + sliceHeight <= kHeight; ++sliceHeightOffset) {
        for (int64_t sliceWidthOffset{0}; sliceWidthOffset + sliceWidth <= kWidth; ++sliceWidthOffset) {
          INFO("sliceHeightOffset: " << sliceHeightOffset << ", sliceWidthOffset: :" << sliceWidthOffset
                                     << ", sliceHeight: " << sliceHeight << ", sliceWidth: " << sliceWidth);
          {
            INFO("Slice goes from top left to bottom right");
            MatrixSlice slice{matrix, sliceHeightOffset, sliceWidthOffset, sliceHeight, sliceWidth};
            checkPartialSliceAndMatrix(slice, matrix);
          }
          {
            INFO("Slice goes from bottom left to top right");
            MatrixSlice slice{matrix, kHeight - sliceHeight - sliceHeightOffset, sliceWidthOffset, sliceHeight,
                              sliceWidth};
            checkPartialSliceAndMatrix(slice, matrix);
          }
        }
      }
    }
  }
}

TEST_CASE("WriteThroughSlice") {
  static constexpr int64_t kHeight{9};
  static constexpr int64_t kWidth{7};
  TestMatrix matrix{kHeight, kWidth};
  MatrixSlice slice{matrix, 0, 0, kHeight, kWidth};

  for (int64_t row{0}; row < kHeight; ++row) {
    for (int64_t column{0}; column < kWidth; ++column) {
      slice.get(row, column) = row * kWidth + column;
    }
  }

  checkSliceAndMatrix(slice, matrix);
}
} // namespace matrix_connected_components::tests
