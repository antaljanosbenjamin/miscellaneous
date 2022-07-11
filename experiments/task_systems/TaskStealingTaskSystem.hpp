#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

#include "NotificationQueue.hpp"

template <size_t K>
class TaskStealingTaskSystem {
  const unsigned m_count{std::thread::hardware_concurrency()};
  std::vector<std::thread> m_threads;
  std::vector<NotificationQueue> m_queue{m_count};
  std::atomic<unsigned> m_index{0};

  void run(unsigned i) {
    while (true) {
      std::function<void()> f;
      for (unsigned n = 0; n != m_count * K; ++n) {
        if (m_queue[(i + n) % m_count].try_pop(f)) {
          break;
        }
      }
      if (!f && !m_queue[i].pop(f)) {
        break;
      }
      f();
    }
  }

public:
  TaskStealingTaskSystem() {
    for (unsigned n = 0; n != m_count; ++n) {
      m_threads.emplace_back([&, n] { run(n); });
    }
  }

  ~TaskStealingTaskSystem() {
    for (auto &e: m_queue) {
      e.done();
    }
    for (auto &e: m_threads) {
      e.join();
    }
  }

  TaskStealingTaskSystem(const TaskStealingTaskSystem &) = delete;
  TaskStealingTaskSystem(TaskStealingTaskSystem &&) = delete;
  TaskStealingTaskSystem &operator=(const TaskStealingTaskSystem &) = delete;
  TaskStealingTaskSystem &operator=(TaskStealingTaskSystem &&) = delete;

  template <typename F>
  void async(F &&f) {
    auto i = m_index++;
    for (unsigned n = 0; n != m_count * K; ++n) {
      if (m_queue[(i + n) % m_count].try_push(std::forward<F>(f))) {
        return;
      }
    }
    m_queue[i % m_count].push(std::forward<F>(f)); // NOLINT(clang-analyzer-core.DivideZero)
  }
};
