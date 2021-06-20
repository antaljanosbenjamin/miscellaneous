#pragma once

namespace ColorFiner {

#if defined(TI_IS_MSVC) || defined(TI_IS_CLANG_CL) || defined(TI_IS_CLANG)
  #define CF_CONSTEXPR constexpr // NOLINT(cppcoreguidelines-macro-usage)
  #define CF_USE_CONSTEXPR true  // NOLINT(cppcoreguidelines-macro-usage)
#else
  #define CF_CONSTEXPR
  #define CF_USE_CONSTEXPR false // NOLINT(cppcoreguidelines-macro-usage)
#endif

constexpr bool useConstexpr = CF_USE_CONSTEXPR;
} // namespace ColorFiner
