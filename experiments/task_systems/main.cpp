#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "MultiQueuedTaskSystem.hpp"
#include "OracleTaskSystem.hpp"
#include "TaskStealingTaskSystem.hpp"

#ifdef MY_DEBUG
std::atomic<uint64_t> NOT_USED;
std::atomic<int> task_count{0};
std::mutex mutex;
#endif

template <typename TaskSystemType>
void AddTasks(TaskSystemType &ts, std::vector<uint64_t> &runningTimes) {
  for (auto runningTime: runningTimes) {
    ts.async([runningTimeInNs = runningTime]() {
      auto start = std::chrono::steady_clock::now();
      auto running_time = std::chrono::steady_clock::duration(runningTimeInNs);
      auto result = runningTimeInNs;
      while ((std::chrono::steady_clock::now() - start) < running_time) {
        const auto firstMagicNumber = 43;
        result *= runningTimeInNs;
        result %= firstMagicNumber;
        const auto secondMagicNumber = 53;
        result *= runningTimeInNs;
        result %= secondMagicNumber;
        const auto thirdMagicNumber = 73;
        result *= runningTimeInNs;
        result %= thirdMagicNumber;
      }
#ifdef MY_DEBUG
      NOT_USED.fetch_add(result, std::memory_order_relaxed);
      task_count.fetch_add(1, std::memory_order_relaxed);
#endif
    });
  }
}

template <typename TaskSystemType>
std::chrono::high_resolution_clock::duration MeasureTaskSystem(std::vector<uint64_t> &runningTimes) {
  auto ts = std::make_unique<TaskSystemType>();
  auto start = std::chrono::high_resolution_clock::now();
  AddTasks(*ts, runningTimes);
  ts = nullptr;
  auto end = std::chrono::high_resolution_clock::now();
  return end - start;
}

int main(int argc, char **argv) {
  const auto numberOfRequiredArguments = 5;
  if (argc < numberOfRequiredArguments) {
    std::cerr << "Please add 4 number as arguments!\n";
    return -1;
  }
  char **endPtr = nullptr;
  constexpr auto base = 10;
  const uint64_t distMin =
      std::strtoull(argv[1], endPtr, base); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const uint64_t distMax =
      std::strtoull(argv[2], endPtr, base); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const uint64_t nrTasks =
      std::strtoull(argv[3], endPtr, base); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const uint64_t testCase =
      std::strtoull(argv[4], endPtr, base); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::random_device dev;
  std::mt19937_64 rng(dev());
  std::uniform_int_distribution<std::mt19937_64::result_type> dist(distMin, distMax);

  std::vector<uint64_t> runningTimes;
  runningTimes.reserve(nrTasks);
  for (auto i = 0U; i < nrTasks; ++i) {
    runningTimes.push_back(dist(rng));
  }

  std::chrono::high_resolution_clock::duration elapsedTime;
  if (testCase == 0U) { // NOLINT(readability-magic-numbers)
    elapsedTime = MeasureTaskSystem<OracleTaskSystem>(runningTimes);
  } else if (testCase == 1U) { // NOLINT(readability-magic-numbers)
    elapsedTime = MeasureTaskSystem<MultiQueuedTaskSystem>(runningTimes);
  } else if (testCase == 2U) { // NOLINT(readability-magic-numbers)
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<1>>(runningTimes);
  } else if (testCase == 3U) { // NOLINT(readability-magic-numbers)
    constexpr auto numberOfTries = 5U;
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<numberOfTries>>(runningTimes);
  } else if (testCase == 4U) { // NOLINT(readability-magic-numbers)
    constexpr auto numberOfTries = 10U;
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<numberOfTries>>(runningTimes);
  } else if (testCase == 5U) { // NOLINT(readability-magic-numbers)
    constexpr auto numberOfTries = 50U;
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<numberOfTries>>(runningTimes);
  } else if (testCase == 6U) { // NOLINT(readability-magic-numbers)
    constexpr auto numberOfTries = 100U;
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<numberOfTries>>(runningTimes);
  }
#ifdef MY_DEBUG
  std::cout << "NOT_USED: " << NOT_USED << "\n";
  std::cout << "Runned tasks: " << task_count << "\n";
#endif

  std::cout << "Running time: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() << "ms\n";
}