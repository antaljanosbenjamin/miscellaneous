#include "memory_manager.hpp"

#include <malloc.h>
#include <new>

void CustomMemoryManager::Start() {
  this->numberOfAllocations = 0;
  this->peakAllocatedBytes = 0;
  this->allocatedBytes = 0;
  this->totalAllocatedBytes = 0;
}

void CustomMemoryManager::Stop(Result *result) {
  result->num_allocs = this->numberOfAllocations;
  result->max_bytes_used = this->peakAllocatedBytes;
  result->total_allocated_bytes = this->totalAllocatedBytes;
}

void CustomMemoryManager::trackMemory(size_t size) {
  auto int_size = static_cast<int64_t>(size);
  this->numberOfAllocations += 1;
  this->allocatedBytes += int_size;
  this->totalAllocatedBytes += int_size;
  this->peakAllocatedBytes = std::max(this->peakAllocatedBytes, this->allocatedBytes);
}

void CustomMemoryManager::untrackMemory(size_t size) {
  auto int_size = static_cast<int64_t>(size);
  this->allocatedBytes -= int_size;
}

void CustomMemoryManager::untrackMemory(void *ptr) {
  const auto blockSize = std::invoke([ptr] {
#ifdef _WIN32
    return _msize(ptr);
#else
    return malloc_usable_size(ptr);
#endif
  });
  untrackMemory(blockSize);
}

CustomMemoryManager &getMemoryManager() {
  static CustomMemoryManager manager{};
  return manager;
}
