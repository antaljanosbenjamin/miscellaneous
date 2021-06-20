#pragma once

#include <type_traits>

namespace minesweeper_gui {

template <typename TEnum>
concept Enum = std::is_enum_v<TEnum>;

template <Enum TEnum>
constexpr std::underlying_type_t<TEnum> getNumericValue(const TEnum enumValue) {
  return static_cast<std::underlying_type_t<TEnum>>(enumValue);
}

} // namespace minesweeper_gui