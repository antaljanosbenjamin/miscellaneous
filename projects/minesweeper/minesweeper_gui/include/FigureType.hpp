#pragma once

#include <cstdint>

namespace minesweeper_gui {

enum class FigureType : uint8_t {
  Empty = 0,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Mine,
  WrongMine,
  Flag,
};

constexpr auto figureTypeCount = 12;

} // namespace minesweeper_gui