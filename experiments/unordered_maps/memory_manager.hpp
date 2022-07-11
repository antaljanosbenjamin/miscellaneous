#pragma once

#include <benchmark/benchmark.h>

class CustomMemoryManager : public benchmark::MemoryManager {
public:
  int64_t numberOfAllocations{0};
  int64_t peakAllocatedBytes{0};
  int64_t allocatedBytes{0};
  int64_t totalAllocatedBytes{0};

  void Start() override;

  void Stop(Result *result) override;

  void trackMemory(size_t size);
  void untrackMemory(size_t size);
  void untrackMemory(void *ptr);
};

CustomMemoryManager &getMemoryManager();