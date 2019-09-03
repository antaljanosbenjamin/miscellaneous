#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

#include "NotificationQueue.h"

class TaskStealingTaskSystem {
  const unsigned count_{std::thread::hardware_concurrency()};
  std::vector<std::thread> threads_;
  std::vector<NotificationQueue> q_{count_};
  std::atomic<unsigned> index_{0};

  void run(unsigned i);

public:
  TaskStealingTaskSystem();
  ~TaskStealingTaskSystem();

  template <typename F> void async(F &&f) {
    auto i = index_++;
    for (unsigned n = 0; n != count_ * 8; ++n) {
      if (q_[(i + n) % count_].try_push(std::forward<F>(f)))
        return;
    }
    q_[i % count_].push(std::forward<F>(f));
  }
};