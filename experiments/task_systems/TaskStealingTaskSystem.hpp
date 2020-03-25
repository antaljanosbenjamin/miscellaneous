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
  const unsigned count_{std::thread::hardware_concurrency()};
  std::vector<std::thread> threads_;
  std::vector<NotificationQueue> q_{count_};
  std::atomic<unsigned> index_{0};

  void run(unsigned i) {
    while (true) {
      std::function<void()> f;
      for (unsigned n = 0; n != count_ * K; ++n) {
        if (q_[(i + n) % count_].try_pop(f))
          break;
      }
      if (!f && !q_[i].pop(f))
        break;
      f();
    }
  }

public:
  TaskStealingTaskSystem() {
    for (unsigned n = 0; n != count_; ++n) {
      threads_.emplace_back([&, n] { run(n); });
    }
  }

  ~TaskStealingTaskSystem() {
    for (auto &e: q_)
      e.done();
    for (auto &e: threads_)
      e.join();
  }

  template <typename F>
  void async(F &&f) {
    auto i = index_++;
    for (unsigned n = 0; n != count_ * 8; ++n) {
      if (q_[(i + n) % count_].try_push(std::forward<F>(f)))
        return;
    }
    // cppcheck-suppress accessForwarded
    q_[i % count_].push(std::forward<F>(f));
  }
};