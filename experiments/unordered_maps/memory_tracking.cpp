#include <malloc.h>
#include <new>

#include "memory_manager.hpp"

extern "C" {
void *__real_malloc(size_t size);
void *__real_aligned_alloc(size_t alignment, size_t size);
void *__real_realloc(void *ptr, size_t size);
void *__real_calloc(void *ptr, size_t size);
void __real_free(void *ptr);
}

void *newImplNoexcept(size_t size) noexcept {
  auto *ptr = __real_malloc(size);
  if (nullptr != ptr) {
    getMemoryManager().trackMemory(size);
  }
  return ptr;
}

void *newImplNoexcept(const std::size_t size, const std::align_val_t align) noexcept {
  auto *ptr = __real_aligned_alloc(static_cast<std::size_t>(align), size);
  if (nullptr != ptr) {
    getMemoryManager().trackMemory(size);
  }

  return ptr;
}

void *newImpl(size_t size) {
  auto *ptr = newImplNoexcept(size);
  if (nullptr == ptr) {
    throw std::bad_alloc{};
  }
  return ptr;
}

void *newImpl(const std::size_t size, const std::align_val_t align) {
  auto *ptr = newImplNoexcept(size, align);
  if (nullptr == ptr) {
    throw std::bad_alloc{};
  }
  return ptr;
}

void deleteImpl(void *ptr) noexcept {
  getMemoryManager().untrackMemory(ptr);
  __real_free(ptr);
}

void deleteImpl(void *ptr, size_t size) noexcept {
  getMemoryManager().untrackMemory(size);
  __real_free(ptr);
}

extern "C" {
void *__wrap_malloc(size_t size) {
  return newImpl(size);
}

void *__wrap_aligned_alloc(size_t alignment, size_t size) {
  return newImpl(size, static_cast<std::align_val_t>(alignment));
}

void *__wrap_realloc(void * /*ptr*/, size_t /*size*/) {
  std::terminate();
}

void *__wrap_calloc(void * /*ptr*/, size_t /*size*/) {
  std::terminate();
}

void __wrap_free(void *ptr) {
  deleteImpl(ptr);
}
}

void *operator new(const std::size_t size) {
  return newImpl(size);
}

void *operator new[](const std::size_t size) {
  return newImpl(size);
}

void *operator new(const std::size_t size, const std::align_val_t align) {
  return newImpl(size, align);
}

void *operator new[](const std::size_t size, const std::align_val_t align) {
  return newImpl(size, align);
}

void *operator new(const std::size_t size, const std::nothrow_t & /*unused*/) noexcept {
  return newImplNoexcept(size);
}

void *operator new[](const std::size_t size, const std::nothrow_t & /*unused*/) noexcept {

  return newImplNoexcept(size);
}

void *operator new(const std::size_t size, const std::align_val_t align, const std::nothrow_t & /*unused*/) noexcept {
  return newImplNoexcept(size, align);
}

void *operator new[](const std::size_t size, const std::align_val_t align, const std::nothrow_t & /*unused*/) noexcept {
  return newImplNoexcept(size, align);
}

void operator delete(void *ptr) noexcept {
  deleteImpl(ptr);
}

void operator delete[](void *ptr) noexcept {
  deleteImpl(ptr);
}

void operator delete(void *ptr, const std::align_val_t /*align*/) noexcept {
  deleteImpl(ptr);
}

void operator delete[](void *ptr, const std::align_val_t /*align*/) noexcept {
  deleteImpl(ptr);
}

void operator delete(void *ptr, const std::size_t size) noexcept {
  deleteImpl(ptr, size);
}

void operator delete[](void *ptr, const std::size_t size) noexcept {
  deleteImpl(ptr, size);
}

void operator delete(void *ptr, const std::size_t size, const std::align_val_t /*align*/) noexcept {
  deleteImpl(ptr, size);
}

void operator delete[](void *ptr, const std::size_t size, const std::align_val_t /*align*/) noexcept {
  deleteImpl(ptr, size);
}

void operator delete(void *ptr, const std::nothrow_t & /*unused*/) noexcept {
  deleteImpl(ptr);
}

void operator delete[](void *ptr, const std::nothrow_t & /*unused*/) noexcept {
  deleteImpl(ptr);
}

void operator delete(void *ptr, const std::align_val_t /*align*/, const std::nothrow_t & /*unused*/) noexcept {
  deleteImpl(ptr);
}

void operator delete[](void *ptr, const std::align_val_t /*align*/, const std::nothrow_t & /*unused*/) noexcept {
  deleteImpl(ptr);
}
