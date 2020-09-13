#pragma once

namespace ColorFiner {

#if defined(_MSC_VER) || defined(__clang__)
#define CF_CONSTEXPR constexpr // NOLINT(cppcoreguidelines-macro-usage)
#define CF_USE_CONSTEXPR true  // NOLINT(cppcoreguidelines-macro-usage)
#else
#define CF_CONSTEXPR
#define CF_USE_CONSTEXPR false // NOLINT(cppcoreguidelines-macro-usage)
#endif

constexpr bool useConstexpr = CF_USE_CONSTEXPR;
} // namespace ColorFiner
