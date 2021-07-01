#pragma once

#include <concepts>
#include <cstddef>
#include <stdexcept>
#include "utils/Assert.hpp"

namespace utils {

enum class ErrorReportType {
  Assert,
  Exception,
};

template <std::equality_comparable_with<std::nullptr_t> TPtr, ErrorReportType reportType = ErrorReportType::Assert>
class NotNull {
public:
  // NOLINTNEXTLINE(google-explicit-constructor)
  NotNull(TPtr ptr) noexcept(reportType == ErrorReportType::Assert)
    : m_ptr{ptr} {
    if constexpr (reportType == ErrorReportType::Assert) {
      MY_ASSERT(nullptr != m_ptr, "nullptr in NotNull");
    } else {
      throw std::logic_error("NotNull cannot be constructed using nullptr!");
    }
  }

  NotNull(std::nullptr_t) = delete;

  NotNull(const NotNull &other)
    : m_ptr{other.m_ptr} {
  }

  // NOLINTNEXTLINE(cert-oop54-cpp) self assignment is no problem here
  NotNull &operator=(const NotNull &other) {
    m_ptr = other.m_ptr;
  }

  NotNull(NotNull &&) = delete;
  NotNull &operator=(NotNull &&) = delete;

  ~NotNull() = default;

  decltype(auto) operator->() const {
    return get();
  }

  decltype(auto) operator*() const {
    return *get();
  }

  [[nodiscard]] decltype(auto) get() const {
    return m_ptr;
  }

private:
  TPtr m_ptr;
};
} // namespace utils