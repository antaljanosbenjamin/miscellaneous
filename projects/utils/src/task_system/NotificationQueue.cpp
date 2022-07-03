#include "utils/task_system/NotificationQueue.hpp"

namespace utils::task_system {
void NotificationQueue::done() {
  {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_done = true;
  }
  m_ready.notify_all();
}

bool NotificationQueue::try_pop(std::function<void()> &x) {
  std::unique_lock<std::mutex> lock{m_mutex, std::try_to_lock};
  if (!lock || m_queue.empty()) {
    return false;
  }
  x = move(m_queue.front());
  m_queue.pop_front();
  return true;
}

bool NotificationQueue::pop(std::function<void()> &x) {
  std::unique_lock<std::mutex> lock{m_mutex};
  while (m_queue.empty() && !m_done) {
    m_ready.wait(lock);
  }
  if (m_queue.empty()) {
    return false;
  }
  x = move(m_queue.front());
  m_queue.pop_front();
  return true;
}
} // namespace utils::task_system