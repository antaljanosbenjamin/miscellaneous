#pragma once

#include <concepts>

namespace utils {

template <typename T>
concept NumericIntegral = std::integral<T> && !std::same_as<T, bool>;

} // namespace utils
