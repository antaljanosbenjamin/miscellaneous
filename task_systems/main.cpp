#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <memory>
#include <vector>

#include "MultiQueuedTaskSystem.h"
#include "OracleTaskSystem.h"
#include "TaskStealingTaskSystem.h"

#ifdef MY_DEBUG
std::atomic<int> NOT_USED;
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
        result *= runningTimeInNs;
        result %= 43;
        result *= runningTimeInNs;
        result %= 53;
        result *= runningTimeInNs;
        result %= 73;
      }
#ifdef MY_DEBUG
      NOT_USED.fetch_add(result, std::memory_order::memory_order_relaxed);
      task_count.fetch_add(1, std::memory_order::memory_order_relaxed);
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
  if (argc < 5) {
    std::cerr << "Please add 4 number as arguments!\n";
    return -1;
  }
  char **endPtr = nullptr;
  const uint64_t distMin = std::strtoull(argv[1], endPtr, 10);
  const uint64_t distMax = std::strtoull(argv[2], endPtr, 10);
  const uint64_t nrTasks = std::strtoull(argv[3], endPtr, 10);
  const uint64_t testCase = std::strtoull(argv[4], endPtr, 10);
  std::random_device dev;
  std::mt19937_64 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(distMin, distMax);

  std::vector<uint64_t> runningTimes;
  runningTimes.reserve(nrTasks);
  for (auto i = 0u; i < nrTasks; ++i) {
    runningTimes.push_back(dist(rng));
  }

  std::chrono::high_resolution_clock::duration elapsedTime;
  if (testCase == 0u) {
    elapsedTime = MeasureTaskSystem<OracleTaskSystem>(runningTimes);
  } else if (testCase == 1u) {
    elapsedTime = MeasureTaskSystem<MultiQueuedTaskSystem>(runningTimes);
  } else if (testCase == 2u) {
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<1>>(runningTimes);
  } else if (testCase == 3u) {
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<5>>(runningTimes);
  } else if (testCase == 4u) {
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<10>>(runningTimes);
  } else if (testCase == 5u) {
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<50>>(runningTimes);
  } else if (testCase == 6u) {
    elapsedTime = MeasureTaskSystem<TaskStealingTaskSystem<100>>(runningTimes);
  }
#ifdef MY_DEBUG
  std::cout << "NOT_USED: " << NOT_USED << "\n";
  std::cout << "Runned tasks: " << task_count << "\n";
#endif

  std::cout << "Running time: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() << "ms\n";
}