#include <catch2/catch.hpp>
#include "utils/NotNull.hpp"

TEST_CASE("NotNullWithNull") {
  auto createNotNullWithNull = []() { utils::NotNull<int *, utils::ErrorReportType::Exception> test{nullptr}; };
  CHECK_THROWS_WITH(createNotNullWithNull(), "NotNull cannot be constructed using nullptr!");
}