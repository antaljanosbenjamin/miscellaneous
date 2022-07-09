#include <catch2/catch.hpp>

#include "utils/containers/DisjointSet.hpp"
#include "utils/containers/ValueTypeOf.hpp"

namespace utils::containers::tests {

static_assert(std::same_as<ValueTypeOf<DisjointSet<int64_t>>, int64_t>, "ValueTypeOf doesn't work with DisjointSet");
static_assert(std::same_as<ValueTypeOf<DisjointSet<uint64_t>>, uint64_t>, "ValueTypeOf doesn't work with DisjointSet");

// Very similar functions can be found in other container tests, but because of the very slight differences, it is hard
// to unify them. Instead of centralizing the functions with complex logic for customizations, they are just copied and
// modified.
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
  CHECK(0 == copiedDs.size()); // NOLINT(bugprone-use-after-move)
}

// Requires copy ctor
template <typename TDisjointSet, typename TCheck>
void TestWithMoveAssignment(const TDisjointSet &ds, TCheck check) {
  INFO("MoveAssignment");
  auto copiedDs = ds;
  TDisjointSet movedDs;
  movedDs = std::move(copiedDs);
  check(movedDs);
  CHECK(0 == copiedDs.size()); // NOLINT(bugprone-use-after-move)
}

template <typename TDisjointSet, typename TCheck>
void TestWithSpecialMemberFunctions(TDisjointSet &ds, TCheck check) {
  TestWithCopyCtor(ds, check);
  TestWithCopyAssignment(ds, check);
  TestWithMoveCtor(ds, check);
  TestWithMoveAssignment(ds, check);
  check(ds);
}

TEST_CASE("EmptyDisjointSet") {
  DisjointSet<int64_t> ds;
  CHECK(0 == ds.size());

  const auto check = [](auto &ds) { CHECK(!ds.find(1).has_value()); };
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
    CHECK(ds.find(kFirstValue) == kFirstValue);
    CHECK(ds.find(kSecondValue) == kSecondValue);
    CHECK(!ds.find(kFirstValue - 1).has_value());
    CHECK(!ds.find(kFirstValue + 1).has_value());
    CHECK(!ds.find(kSecondValue - 1).has_value());
    CHECK(!ds.find(kSecondValue + 1).has_value());
  };
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
  TestWithSpecialMemberFunctions(ds, check);
}

TEST_CASE("SimpleMerge") {
  DisjointSet<int64_t> ds;
  using Values = std::pair<int64_t, int64_t>;
  const auto values = GENERATE(Values{24, 42}, Values{5, 3});
  const auto firstValue = values.first;
  const auto secondValue = values.second;
  const auto expected = std::min(firstValue, secondValue);

  ds.add(firstValue);
  CHECK(firstValue == ds.find(firstValue));

  ds.add(secondValue);
  CHECK(secondValue == ds.find(secondValue));

  CHECK(ds.merge(firstValue, secondValue));
  const auto check = [firstValue, secondValue, expected](auto &ds) {
    CHECK(2 == ds.size());

    CHECK(ds.find(firstValue) == expected);
    CHECK(ds.find(secondValue) == expected);
    CHECK(!ds.find(firstValue - 1).has_value());
    CHECK(!ds.find(firstValue + 1).has_value());
    CHECK(!ds.find(secondValue - 1).has_value());
    CHECK(!ds.find(secondValue + 1).has_value());

    CHECK(!ds.merge(firstValue, secondValue));
  };
  TestWithSpecialMemberFunctions(ds, check);
}

TEST_CASE("DoubleMerge") {
  DisjointSet<int64_t> ds;
  using Values = std::tuple<int64_t, int64_t, int64_t>;
  const auto values = GENERATE(Values{10, 100, 1000}, Values{10, 1000, 100}, Values{100, 10, 1000},
                               Values{100, 1000, 10}, Values{1000, 10, 100}, Values{1000, 100, 10});
  const auto firstValue = std::get<0>(values);
  const auto secondValue = std::get<1>(values);
  const auto thirdValue = std::get<2>(values);
  const auto expected = std::min(std::min(firstValue, secondValue), thirdValue);

  ds.add(firstValue);
  CHECK(firstValue == ds.find(firstValue));

  ds.add(secondValue);
  CHECK(secondValue == ds.find(secondValue));

  ds.add(thirdValue);
  CHECK(thirdValue == ds.find(thirdValue));

  CHECK(ds.merge(firstValue, secondValue));
  CHECK(ds.merge(secondValue, thirdValue));
  const auto check = [firstValue, secondValue, thirdValue, expected](auto &ds) {
    CHECK(3 == ds.size());

    CHECK(ds.find(firstValue) == expected);
    CHECK(ds.find(secondValue) == expected);
    CHECK(ds.find(thirdValue) == expected);

    CHECK(!ds.merge(firstValue, secondValue));
    CHECK(!ds.merge(secondValue, thirdValue));
    CHECK(!ds.merge(firstValue, thirdValue));
  };
  TestWithSpecialMemberFunctions(ds, check);
}
} // namespace utils::containers::tests
