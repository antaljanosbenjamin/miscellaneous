#pragma once

#if defined(TI_IS_CLANG) || defined(TI_IS_CLANG_CL)
  // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
  #define MY_LIKELY(x) __builtin_expect(!!(x), 1)
  // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
  #define MY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
  // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
  #define MY_LIKELY(x) [[likely]] x
  // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
  #define MY_UNLIKELY(x) [[unlikely]] x
#endif