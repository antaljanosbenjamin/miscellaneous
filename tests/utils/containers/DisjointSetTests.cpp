#include <catch2/catch.hpp>
#include "utils/containers/DisjointSet.hpp"

namespace utils::containers::tests {

template <typename TDisjointSet, typename TCheck>
void TestWithCopyCtor(const TDisjointSet &ds, TCheck check) {
  INFO("CopyCtor");
  auto copiedDs = ds;
  check(copiedDs);
}

template <typename TDisjointSet, typename TCheck>
void TestWithCopyAssignment(const TDisjointSet &ds, TCheck check) {
  INFO("CopyAssignment");
  TDisjointSet copiedDs;
  copiedDs = ds;
  check(copiedDs);
}

// Requires copy ctor
template <typename TDisjointSet, typename TCheck>
void TestWithMoveCtor(const TDisjointSet &ds, TCheck check) {
  INFO("MoveCtor");
  auto copiedDs = ds;
  auto movedDs = std::move(copiedDs);
  check(movedDs);
  CHECK(0 == copiedDs.size());
}

// Requires copy ctor
template <typename TDisjointSet, typename TCheck>
void TestWithMoveAssignment(const TDisjointSet &ds, TCheck check) {
  INFO("MoveAssignment");
  auto copiedDs = ds;
  TDisjointSet movedDs;
  movedDs = std::move(copiedDs);
  check(movedDs);
  CHECK(0 == copiedDs.size());
}

template <typename TDisjointSet, typename TCheck>
void TestWithSpecialMemberFunctions(const TDisjointSet &ds, TCheck check) {
  TestWithCopyCtor(ds, check);
  TestWithCopyAssignment(ds, check);
  TestWithMoveCtor(ds, check);
  TestWithMoveAssignment(ds, check);
}

TEST_CASE("EmptyDisjointSet") {
  DisjointSet<int64_t> ds;
  CHECK(0 == ds.size());

  const auto check = [](auto &ds) { CHECK(!ds.find(1).has_value()); };
  check(ds);
  TestWithSpecialMemberFunctions(ds, check);
}

TEST_CASE("AddSingle") {
  DisjointSet<int64_t> ds;
  ds.add(1);

  const auto check = [](auto &ds) {
    CHECK(1 == ds.size());
    CHECK(1 == ds.find(1));
    CHECK(!ds.find(2).has_value());
  };
  check(ds);
  TestWithSpecialMemberFunctions(ds, check);
}

TEST_CASE("AddTwo") {
  DisjointSet<int64_t> ds;
  static constexpr int64_t kFirstValue = 0;
  static constexpr int64_t kSecondValue = 42;

  ds.add(kFirstValue);
  CHECK(kFirstValue == ds.find(kFirstValue));

  ds.add(kSecondValue);
  CHECK(kSecondValue == ds.find(kSecondValue));

  const auto check = [](auto &ds) {
    CHECK(2 == ds.size());

    CHECK(!ds.find(kFirstValue - 1).has_value());
    CHECK(!ds.find(kFirstValue + 1).has_value());
    CHECK(!ds.find(kSecondValue - 1).has_value());
    CHECK(!ds.find(kSecondValue + 1).has_value());
  };
  check(ds);
  TestWithSpecialMemberFunctions(ds, check);
}

TEST_CASE("MultipleAdd") {
  DisjointSet<int64_t> ds;
  static constexpr int64_t kStartValue = -5000;
  static constexpr int64_t kEndValue = 5000;
  static constexpr int64_t kIncrement = 37;

  int64_t expectedSize{0};
  for (auto i = kStartValue; i < kEndValue; i += kIncrement) {
    CHECK(expectedSize == ds.size());
    CHECK(!ds.find(i).has_value());
    ds.add(i);
    ++expectedSize;
    CHECK(expectedSize == ds.size());
    CHECK(ds.find(i).has_value());
  }

  const auto check = [expectedSize](auto &ds) {
    CHECK(expectedSize == ds.size());
    for (auto i = kStartValue; i < kEndValue; i += kIncrement) {
      CHECK(ds.find(i).has_value());
      CHECK(!ds.find(i - 1).has_value());
      CHECK(!ds.find(i + 1).has_value());
    }
  };
  check(ds);
  TestWithSpecialMemberFunctions(ds, check);
}
} // namespace utils::containers::tests
