#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

#include "NotificationQueue.hpp"

class MultiQueuedTaskSystem {
  const unsigned count_{std::thread::hardware_concurrency()};
  std::vector<std::thread> threads_;
  std::vector<NotificationQueue> q_{count_};
  std::atomic<unsigned> index_{0};

  void run(unsigned i);

public:
  MultiQueuedTaskSystem();
  ~MultiQueuedTaskSystem();

  MultiQueuedTaskSystem(const MultiQueuedTaskSystem &) = delete;
  MultiQueuedTaskSystem(MultiQueuedTaskSystem &&) = delete;
  MultiQueuedTaskSystem &operator=(const MultiQueuedTaskSystem &) = delete;
  MultiQueuedTaskSystem &operator=(MultiQueuedTaskSystem &&) = delete;

  template <typename F>
  void async(F &&f) {
    auto i = index_++;
    q_[i % count_].push(std::forward<F>(f));
  }
};