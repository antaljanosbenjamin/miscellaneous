#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

namespace utils::task_system {
class NotificationQueue {
  std::deque<std::function<void()>> m_queue{};
  bool m_done{false};
  std::mutex m_mutex{};
  std::condition_variable m_ready{};

public:
  void done();
  bool pop(std::function<void()> &x);
  bool try_pop(std::function<void()> &x);

  template <typename F>
  void push(F &&f) {
    {
      std::unique_lock<std::mutex> lock{m_mutex};
      m_queue.emplace_back(std::forward<F>(f));
    }
    m_ready.notify_one();
  }

  template <typename F>
  bool try_push(F &&f) {
    {
      std::unique_lock<std::mutex> lock{m_mutex, std::try_to_lock};
      if (!lock) {
        return false;
      }
      m_queue.emplace_back(std::forward<F>(f));
    }
    m_ready.notify_one();
    return true;
  }
};
} // namespace utils::task_system