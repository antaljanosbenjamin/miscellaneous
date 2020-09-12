#pragma once

namespace ColorFiner {

#if defined(_MSC_VER) || defined(__clang__)
#define CF_CONSTEXPR constexpr
#define CF_USE_CONSTEXPR true
#else
#define CF_CONSTEXPR
#define CF_USE_CONSTEXPR false
#endif

constexpr bool useConstexpr = CF_USE_CONSTEXPR;
} // namespace ColorFiner
