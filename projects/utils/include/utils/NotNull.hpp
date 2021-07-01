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
  NotNull(TPtr ptr)
    : m_ptr{ptr} {
    if constexpr (reportType == ErrorReportType::Assert) {
      MY_ASSERT(nullptr != m_ptr, "nullptr in NotNull");
    } else {
      throw std::logic_error("NotNull cannot be constructed using nullptr!");
    }
  }

  NotNull(const NotNull &other)
    : m_ptr{other.m_ptr} {
  }

  NotNull &operator=(const NotNull &other) {
    m_ptr = other.m_ptr;
  }
  ~NotNull() = default;

  decltype(auto) operator->() const {
    return get();
  }

  decltype(auto) operator*() const {
    return *get();
  }

  decltype(auto) get() const {
    return m_ptr;
  }

private:
  TPtr m_ptr;
};
} // namespace utils