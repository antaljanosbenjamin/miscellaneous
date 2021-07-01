#include <catch2/catch.hpp>
#include "utils/NotNull.hpp"

TEST_CASE("NotNullWithNull") {
  auto createNotNullWithNull = []() {
    int *null = nullptr;
    utils::NotNull<int *, utils::ErrorReportType::Exception> test{null};
  };
  CHECK_THROWS_WITH(createNotNullWithNull(), "NotNull cannot be constructed using nullptr!");
}