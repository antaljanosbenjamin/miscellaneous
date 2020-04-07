#include "MultiQueuedTaskSystem.hpp"

void MultiQueuedTaskSystem::run(unsigned i) {
  while (true) {
    std::function<void()> f;
    if (!q_[i].pop(f)) {
      break;
    }
    f();
  }
}

MultiQueuedTaskSystem::MultiQueuedTaskSystem() {
  for (unsigned n = 0; n != count_; ++n) {
    threads_.emplace_back([&, n] { run(n); });
  }
}

MultiQueuedTaskSystem::~MultiQueuedTaskSystem() {
  for (auto &e: q_) {
    e.done();
  }
  for (auto &e: threads_) {
    e.join();
  }
}
