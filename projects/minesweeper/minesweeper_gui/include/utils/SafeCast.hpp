#pragma once

#include <type_traits>

namespace minesweeper_gui {

template <typename TTo, typename TFrom>
constexpr TTo safeCast(const TFrom &from) {
  // TODO(antaljanosbenjamin) create real implementation
  return static_cast<TTo>(from);
}

} // namespace minesweeper_gui