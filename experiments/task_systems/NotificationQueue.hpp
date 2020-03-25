#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

// cppcheck-suppress noConstructor
class NotificationQueue {
  std::deque<std::function<void()>> q_{};
  bool done_{false};
  std::mutex mutex_{};
  std::condition_variable ready_{};

public:
  void done();
  bool pop(std::function<void()> &x);
  bool try_pop(std::function<void()> &x);

  template <typename F>
  void push(F &&f) {
    {
      std::unique_lock<std::mutex> lock{mutex_};
      q_.emplace_back(std::forward<F>(f));
    }
    ready_.notify_one();
  }

  template <typename F>
  bool try_push(F &&f) {
    {
      std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock};
      if (!lock)
        return false;
      q_.emplace_back(std::forward<F>(f));
    }
    ready_.notify_one();
    return true;
  }
};