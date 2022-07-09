#include <bit>
#include <iostream>
#include <map>
#include <random>
#include <unordered_map>

#include <benchmark/benchmark.h>
#include <robin_hood.h>

#include "memory_manager.hpp"

template <typename T, typename THash = robin_hood::hash<T>>
struct PairHash {
  size_t operator()(const std::pair<T, T> &p) const {
    static constexpr size_t fnv_prime = 1099511628211UL;
    static constexpr size_t fnv_offset = 14695981039346656037UL;
    size_t ret = fnv_offset;
    ret ^= THash()(p.first);
    ret *= fnv_prime;
    ret ^= THash()(p.second);
    return ret;
  }
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SIZE_TYPE int64_t
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LABEL_TYPE int64_t

template <typename T>
struct StringRep {};

template <>
struct StringRep<int32_t> {
  static constexpr std::string_view value{"int32_t"};
};

template <>
struct StringRep<int64_t> {
  static constexpr std::string_view value{"int64_t"};
};

template <typename TMap, SIZE_TYPE kMaxDimension, SIZE_TYPE kFilledItems>
static void Map(benchmark::State &state) {
  std::mt19937 mt(1); // NOLINTNEXTLINE(cert-msc32-c)
  std::uniform_int_distribution<SIZE_TYPE> r1(0, kMaxDimension - 1);
  TMap map;
  for (auto i = 0ULL; i < kFilledItems; ++i) {
    map.emplace(std::make_pair(r1(mt), r1(mt)), 1);
  }
  // Code inside this loop is measured repeatedly
  for (auto _: state) {
    // Make sure the variable is not optimized away by compiler
    const auto key = std::make_pair(r1(mt), r1(mt));
    benchmark::DoNotOptimize(map.contains(key));
  }
}

template <typename T>
bool operator<(const std::pair<T, T> &a, const std::pair<T, T> &b) {
  return (a.first < b.first) || (a.first == b.first && a.second < b.second);
}

template <typename TVector, size_t kMaxDimension, size_t kFilledItems>
static void Vector(benchmark::State &state) {
  std::mt19937 mt(1); // NOLINTNEXTLINE(cert-msc32-c)
  std::uniform_int_distribution<SIZE_TYPE> r1(0, kMaxDimension - 1);
  TVector vec(kMaxDimension * kMaxDimension, 0);
  for (auto _: state) {
    const auto key = std::make_pair(r1(mt), r1(mt));
    benchmark::DoNotOptimize(vec[key.first * kMaxDimension + key.second]);
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define BENCH(kMaxDimension, kFilledItems)                                                                             \
  BENCHMARK_TEMPLATE(Vector, std::vector<LABEL_TYPE>, kMaxDimension, kFilledItems);                                    \
  BENCHMARK_TEMPLATE(Map, std::unordered_map<std::pair<SIZE_TYPE, SIZE_TYPE>, LABEL_TYPE, PairHash<uint32_t>>,         \
                     kMaxDimension, kFilledItems);                                                                     \
  BENCHMARK_TEMPLATE(Map, std::map<std::pair<SIZE_TYPE, SIZE_TYPE>, LABEL_TYPE>, kMaxDimension, kFilledItems);         \
  BENCHMARK_TEMPLATE(Map,                                                                                              \
                     robin_hood::unordered_flat_map<std::pair<SIZE_TYPE, SIZE_TYPE>, LABEL_TYPE, PairHash<uint32_t>>,  \
                     kMaxDimension, kFilledItems);                                                                     \
  BENCHMARK_TEMPLATE(Map,                                                                                              \
                     robin_hood::unordered_node_map<std::pair<SIZE_TYPE, SIZE_TYPE>, LABEL_TYPE, PairHash<size_t>>,    \
                     kMaxDimension, kFilledItems)

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-avoid-non-const-global-variables)
BENCH(5, 10);
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-avoid-non-const-global-variables)
BENCH(50, 1'000);
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-avoid-non-const-global-variables)
BENCH(500, 100'000);
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-avoid-non-const-global-variables)
BENCH(5'000, 10'000'000);
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory,cppcoreguidelines-avoid-non-const-global-variables)
BENCHMARK_TEMPLATE(Vector, std::vector<LABEL_TYPE>, 50'000, 1'000'000'000);

int main(int argc, char **argv) {
  std::cout << "Size type is " << StringRep<SIZE_TYPE>::value << std::endl;
  std::cout << "Label type is " << StringRep<LABEL_TYPE>::value << std::endl;
  ::benchmark::RegisterMemoryManager(&getMemoryManager());
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::RegisterMemoryManager(nullptr);
}
