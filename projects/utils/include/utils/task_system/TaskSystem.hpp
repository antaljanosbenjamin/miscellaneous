#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

#include "utils/task_system/NotificationQueue.hpp"

namespace utils::task_system {
// The implementation is based on Sean Parent's task stealing task system presented in Better Code: Concurrency
// https://sean-parent.stlab.cc/presentations/2016-08-08-concurrency/2016-08-08-concurrency.pdf
template <size_t kNumberOfTrialIterations>
class TaskSystemBase {
  const uint64_t m_count{std::thread::hardware_concurrency()};
  std::vector<std::thread> m_threads;
  std::vector<NotificationQueue> m_queue{m_count};
  std::atomic<uint64_t> m_index{0};

  void run(const uint64_t startIndex) {
    while (true) {
      std::function<void()> task;
      for (auto i = 0U; i != this->m_count * kNumberOfTrialIterations; ++i) {
        if (this->m_queue[(startIndex + i) % this->m_count].try_pop(task)) {
          break;
        }
      }
      if (!task && !this->m_queue[startIndex].pop(task)) {
        break;
      }
      task();
    }
  }

public:
  TaskSystemBase() {
    for (auto i = 0U; i != m_count; ++i) {
      m_threads.emplace_back([&, i] { run(i); });
    }
  }

  ~TaskSystemBase() {
    for (auto &e: m_queue) {
      e.done();
    }
    for (auto &e: m_threads) {
      e.join();
    }
  }

  TaskSystemBase(const TaskSystemBase &) = delete;
  TaskSystemBase(TaskSystemBase &&) = delete;
  TaskSystemBase &operator=(const TaskSystemBase &) = delete;
  TaskSystemBase &operator=(TaskSystemBase &&) = delete;

  template <typename TFunc>
  void async(TFunc &&task) {
    auto i = m_index++;
    for (unsigned n = 0; n != m_count * kNumberOfTrialIterations; ++n) {
      if (m_queue[(i + n) % m_count].try_push(std::forward<TFunc>(task))) {
        return;
      }
    }
    m_queue[i % m_count].push(std::forward<TFunc>(task)); // NOLINT(clang-analyzer-core.DivideZero)
  }
};

static constexpr size_t kDefaultTrialIterations = 8;
using TaskSystem = TaskSystemBase<kDefaultTrialIterations>;
} // namespace utils::task_system
