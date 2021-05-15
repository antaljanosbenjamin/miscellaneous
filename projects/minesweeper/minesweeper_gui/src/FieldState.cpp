#include "FieldState.hpp"

#include <tl/expected.hpp>

namespace minesweeper_gui {
FieldState::FieldState(const uint64_t row, const uint64_t column)
  : baseState{BaseState::Closed}
  , figureType{FigureType::Empty}
  , row{row}
  , column{column} {
}

void FieldState::toggleFlag(minesweeper::Minesweeper &game) {
  game.toggleFlag(row, column).and_then([this](const minesweeper::Result<minesweeper::FieldFlagResult> &result) {
    switch (*result) {
    case minesweeper::FieldFlagResult::Flagged:
      this->updateWithFigureType(FigureType::Flag);
      break;
    case minesweeper::FieldFlagResult::FlagRemoved:
      this->updateWithFigureType(FigureType::Empty);
      break;
    case minesweeper::FieldFlagResult::AlreadyOpened:
      break;
    }
    return minesweeper::Result<void>();
  });
}

std::vector<minesweeper::OpenedField> FieldState::open(minesweeper::Minesweeper &game) {
  return game.open(row, column)
      .and_then([this](minesweeper::Result<minesweeper::OpenInfo> &&result) {
        auto &openInfo = *result;

        return minesweeper::Result<std::vector<minesweeper::OpenedField>>(std::move(openInfo.newlyOpenedFields));
      })
      .value();
}

void FieldState::updateWithBaseState(const BaseState &newBaseState) {
  if (newBaseState == BaseState::Hoovered && !canBeHoover()) {
    return;
  }
  if (newBaseState == BaseState::Closed && !isHoovered()) {
    return;
  }
  baseState = newBaseState;
}

void FieldState::updateWithFigureType(const FigureType &newFigureType) {
  if (newFigureType == FigureType::Flag && baseState != BaseState::Closed) {
    return;
  }

  figureType = newFigureType;
}

void FieldState::draw(wxDC &dc, const FieldBitmaps &bitmaps) const {
  dc.DrawBitmap(getBackground(bitmaps), 0, 0, false);
  if (shouldDrawFigure()) {
    dc.DrawBitmap(getFigure(bitmaps), 0, 0, false);
  }
}

[[nodiscard]] bool FieldState::canBeHoover() const {
  return baseState == BaseState::Closed;
}

[[nodiscard]] bool FieldState::isHoovered() const {
  return baseState == BaseState::Hoovered;
}

[[nodiscard]] bool FieldState::shouldDrawFigure() const {
  return baseState == BaseState::Boomed || baseState == BaseState::Opened || figureType == FigureType::Flag;
}

[[nodiscard]] const wxBitmap &FieldState::getBackground(const FieldBitmaps &bitmaps) const {
  return bitmaps.getBackground(baseState);
}

[[nodiscard]] const wxBitmap &FieldState::getFigure(const FieldBitmaps &bitmaps) const {
  return bitmaps.getFigure(figureType);
}
} // namespace minesweeper_gui
