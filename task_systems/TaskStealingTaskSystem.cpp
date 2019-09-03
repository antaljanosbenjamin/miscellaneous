#include "TaskStealingTaskSystem.h"

void TaskStealingTaskSystem::run(unsigned i) {
  while (true) {
    std::function<void()> f;
    for (unsigned n = 0; n != count_; ++n) {
      if (q_[(i + n) % count_].try_pop(f))
        break;
    }
    if (!f && !q_[i].pop(f))
      break;
    f();
  }
}

TaskStealingTaskSystem::TaskStealingTaskSystem() {
  for (unsigned n = 0; n != count_; ++n) {
    threads_.emplace_back([&, n] { run(n); });
  }
}

TaskStealingTaskSystem::~TaskStealingTaskSystem() {
  for (auto &e: q_)
    e.done();
  for (auto &e: threads_)
    e.join();
}
