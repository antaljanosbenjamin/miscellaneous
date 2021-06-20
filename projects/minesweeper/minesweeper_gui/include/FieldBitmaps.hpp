#pragma once

#include <algorithm>
#include <array>

#include <fmt/core.h>
#include "BaseState.hpp"
#include "FigureType.hpp"
#include "WxWidgetsWrapper.hpp"

namespace minesweeper_gui {

class FieldBitmaps {
public:
  explicit FieldBitmaps(int bitmapSize);
  FieldBitmaps(const FieldBitmaps &) = default;
  FieldBitmaps(FieldBitmaps &&) = default;
  FieldBitmaps &operator=(const FieldBitmaps &) = default;
  FieldBitmaps &operator=(FieldBitmaps &&) = default;
  ~FieldBitmaps() = default;

  [[nodiscard]] const wxBitmap &getBackground(const BaseState &guiState) const;
  [[nodiscard]] wxBitmap &getBackground(const BaseState &guiState);
  [[nodiscard]] const wxBitmap &getFigure(const FigureType &fieldType) const;
  [[nodiscard]] wxBitmap &getFigure(const FigureType &fieldType);
  [[nodiscard]] int getSize() const;

private:
  std::array<wxBitmap, baseStateCount> backgrounds;
  std::array<wxBitmap, figureTypeCount> figures;
};

} // namespace minesweeper_gui