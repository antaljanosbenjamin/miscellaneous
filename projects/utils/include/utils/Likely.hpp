#pragma once

#if defined(TI_IS_CLANG) || defined(TI_IS_CLANG_CL)
  #define MY_LIKELY(x) __builtin_expect(!!(x), 1)
  #define MY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
  #define MY_LIKELY(x) [[likely]] x
  #define MY_UNLIKELY(x) [[unlikely]] x
#endif