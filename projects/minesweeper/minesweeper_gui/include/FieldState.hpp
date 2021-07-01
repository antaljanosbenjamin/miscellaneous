#pragma once

#include <cstdint>

#include <Minesweeper.hpp>
#include "BaseState.hpp"
#include "FieldBitmaps.hpp"
#include "FigureType.hpp"
#include "WxWidgetsWrapper.hpp"
#include "utils/Warnings.hpp"

namespace minesweeper_gui {
class FieldState {
public:
  FieldState(const uint64_t row, const uint64_t column);

  void toggleFlag(minesweeper::Minesweeper &game);
  minesweeper::OpenInfo open(minesweeper::Minesweeper &game);
  void updateWithBaseState(const BaseState &newBaseState);
  void updateWithFigureType(const FigureType &newFigureType);
  void draw(wxDC &dc, const FieldBitmaps &bitmaps) const;

private:
  [[nodiscard]] bool canBeHoover() const;

  [[nodiscard]] bool isHoovered() const;

  [[nodiscard]] bool shouldDrawFigure() const;

  [[nodiscard]] const wxBitmap &getBackground(const FieldBitmaps &bitmaps) const;

  [[nodiscard]] const wxBitmap &getFigure(const FieldBitmaps &bitmaps) const;

  static constexpr auto defaultFigureType = FigureType::Empty;
  BaseState baseState{BaseState::Closed};
  FigureType figureType{defaultFigureType};
  CLANG_MAYBE_UNUSED uint64_t row{0U};    // NOLINT(clang-diagnostic-unused-private-field)
  CLANG_MAYBE_UNUSED uint64_t column{0U}; // NOLINT(clang-diagnostic-unused-private-field)
};
} // namespace minesweeper_gui
