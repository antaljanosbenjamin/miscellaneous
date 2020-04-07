#pragma once

#include <functional>
#include <thread>
#include <vector>

#include "NotificationQueue.hpp"

class OracleTaskSystem {
  const unsigned _count{std::thread::hardware_concurrency()};
  std::vector<std::thread> threads_;
  NotificationQueue q_;

  void run(unsigned /*i*/);

public:
  OracleTaskSystem();
  ~OracleTaskSystem();

  OracleTaskSystem(const OracleTaskSystem &) = delete;
  OracleTaskSystem(OracleTaskSystem &&) = delete;
  OracleTaskSystem &operator=(const OracleTaskSystem &) = delete;
  OracleTaskSystem &operator=(OracleTaskSystem &&) = delete;

  template <typename F>
  void async(F &&f) {
    q_.push(std::forward<F>(f));
  }
};
