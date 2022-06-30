#include <bit>
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

template <typename TMap, size_t kMaxDimension, size_t kFilledItems>
static void Map(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<size_t> r1(0, kMaxDimension - 1);
  TMap map;
  for (auto i = 0ULL; i < kFilledItems; ++i) {
    map.emplace(std::make_pair(r1(mt), r1(mt)), 0);
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

template <size_t kMaxDimension, size_t kFilledItems>
static void Vector(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<size_t> r1(0, kMaxDimension - 1);
  std::vector<uint64_t> vec(kMaxDimension * kMaxDimension, 0);
  for (auto _: state) {
    const auto key = std::make_pair(r1(mt), r1(mt));
    benchmark::DoNotOptimize(vec[key.first * kMaxDimension + key.second]);
  }
}

#define BENCH(kMaxDimension, kFilledItems)                                                                             \
  BENCHMARK_TEMPLATE(Vector, kMaxDimension, kFilledItems);                                                             \
  BENCHMARK_TEMPLATE(Map, std::unordered_map<std::pair<uint32_t, uint32_t>, uint64_t, PairHash<uint32_t>>,             \
                     kMaxDimension, kFilledItems);                                                                     \
  BENCHMARK_TEMPLATE(Map, std::map<std::pair<uint32_t, uint32_t>, uint64_t>, kMaxDimension, kFilledItems);             \
  BENCHMARK_TEMPLATE(Map, robin_hood::unordered_flat_map<std::pair<uint32_t, uint32_t>, uint64_t, PairHash<uint32_t>>, \
                     kMaxDimension, kFilledItems);                                                                     \
  BENCHMARK_TEMPLATE(Map, robin_hood::unordered_node_map<std::pair<uint32_t, uint32_t>, uint64_t, PairHash<size_t>>,   \
                     kMaxDimension, kFilledItems)

BENCH(5, 10);
BENCH(50, 1'000);
BENCH(500, 100'000);
BENCH(5'000, 10'000'000);
BENCHMARK_TEMPLATE(Vector, 50'000, 1'000'000'000);

int main(int argc, char **argv) {
  ::benchmark::RegisterMemoryManager(&getMemoryManager());
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::RegisterMemoryManager(nullptr);
}
