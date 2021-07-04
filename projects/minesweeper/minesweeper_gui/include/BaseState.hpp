#pragma once

#include <cstdint>

namespace minesweeper_gui {

enum class BaseState : uint8_t {
  Closed = 0,
  Hovered,
  Boomed,
  Opened,
};

constexpr auto baseStateCount = 4;

} // namespace minesweeper_gui