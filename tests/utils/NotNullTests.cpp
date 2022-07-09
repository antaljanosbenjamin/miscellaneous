#include <catch2/catch.hpp>
#include "utils/NotNull.hpp"

namespace utils::tests {

static_assert(std::same_as<decltype(std::declval<NotNull<int *>>().get()), int *>);
static_assert(std::same_as<decltype(std::declval<const NotNull<int *>>().get()), int *>);
static_assert(std::same_as<decltype(std::declval<NotNull<const int *>>().get()), const int *>);
static_assert(std::same_as<decltype(std::declval<const NotNull<const int *>>().get()), const int *>);

TEST_CASE("WithNull") {
  auto createNotNullWithNull = []() {
    int *null = nullptr;
    NotNull<int *, ErrorReportType::Exception> test{null};
  };
  CHECK_THROWS_AS(createNotNullWithNull(), std::logic_error);
}

TEST_CASE("ReturnsCorrectPointer") {
  int value{2};
  NotNull notNull{&value};
  value++;
  CHECK(&value == notNull.get());
  CHECK(value == *notNull);
}

TEST_CASE("Arrow") {
  struct Dummy {
    int a;
  };
  Dummy value{2};
  NotNull notNull{&value};
  value.a++;
  CHECK(&value == notNull.get());
  CHECK(value.a == notNull->a);
}
TEST_CASE("CopyCtor") {
  int value{2};
  NotNull original{&value};
  value++;
  NotNull copied{original};
  value++;
  CHECK(&value == original.get());
  CHECK(value == *original);
  CHECK(&value == copied.get());
  CHECK(value == *copied);
}

TEST_CASE("CopyAssignment") {
  int value{2};
  NotNull original{&value};
  value++;
  int dummyValue{-1};
  NotNull copied{&dummyValue};
  copied = original;
  value++;
  CHECK(&value == original.get());
  CHECK(value == *original);
  CHECK(&value == copied.get());
  CHECK(value == *copied);
}

TEST_CASE("MoveCtor") {
  int value{2};
  NotNull original{&value};
  value++;
  NotNull moved{std::move(original)}; // NOLINT(performance-move-const-arg)
  value++;
  CHECK(&value == original.get()); // NOLINT(hicpp-invalid-access-moved)
  CHECK(value == *original);
  CHECK(&value == moved.get());
  CHECK(value == *moved);
}

TEST_CASE("MoveAssignment") {
  int value{2};
  NotNull original{&value};
  value++;
  int dummyValue{-1};
  NotNull moved{&dummyValue};
  moved = std::move(original); // NOLINT(performance-move-const-arg)
  value++;
  CHECK(&value == original.get()); // NOLINT(hicpp-invalid-access-moved)
  CHECK(value == *original);
  CHECK(&value == moved.get());
  CHECK(value == *moved);
}
} // namespace utils::tests
