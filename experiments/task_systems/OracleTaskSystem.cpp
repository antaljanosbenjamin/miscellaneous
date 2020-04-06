#include "OracleTaskSystem.hpp"

void OracleTaskSystem::run(unsigned /*i*/) {
  std::function<void()> f;
  while (q_.pop(f)) {
    f();
  }
}

OracleTaskSystem::OracleTaskSystem() {
  for (unsigned n = 0; n != _count; ++n) {
    threads_.emplace_back([&, n] { run(n); });
  }
}

OracleTaskSystem::~OracleTaskSystem() {
  q_.done();
  for (auto &e: threads_) {
    e.join();
  }
}
