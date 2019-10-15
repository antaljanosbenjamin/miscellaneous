#include "NotificationQueue.hpp"

void NotificationQueue::done() {
  {
    std::unique_lock<std::mutex> lock{mutex_};
    done_ = true;
  }
  ready_.notify_all();
}

bool NotificationQueue::try_pop(std::function<void()> &x) {
  std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock};
  if (!lock || q_.empty())
    return false;
  x = move(q_.front());
  q_.pop_front();
  return true;
}

bool NotificationQueue::pop(std::function<void()> &x) {
  std::unique_lock<std::mutex> lock{mutex_};
  while (q_.empty() && !done_)
    ready_.wait(lock);
  if (q_.empty())
    return false;
  x = move(q_.front());
  q_.pop_front();
  return true;
}