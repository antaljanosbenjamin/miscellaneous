#pragma once

namespace utils::detail::assert {
[[noreturn]] void HandleFailedAssert(const char *filename, int lineNumber, const char *expresssion,
                                     const char *message);
[[noreturn]] void HandleFatal(const char *filename, int lineNumber, const char *message);
} // namespace  utils::detail::assert

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MY_ASSERT(expression, message)                                                                                 \
  [[likely]] (!!(expression)) ? static_cast<void>(0)                                                                   \
                              : utils::detail::assert::HandleFailedAssert(__FILE__, __LINE__, #expression, message);

#define MY_FATAL(message) utils::detail::assert::HandleFatal(__FILE__, __LINE__, message);

#define MY_UNREACHABLE MY_FATAL("Unreachable");