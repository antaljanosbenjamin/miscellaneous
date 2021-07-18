#include "FieldBitmaps.hpp"

#include "utils/EnumHelper.hpp"

namespace minesweeper_gui {

FieldBitmaps::FieldBitmaps(int bitmapSize) {
  std::fill(backgrounds.begin(), backgrounds.end(),
            wxBitmap{bitmapSize, bitmapSize}); // NOLINT(clang-analyzer-optin.cplusplus.VirtualCall)
  std::fill(figures.begin(), figures.end(), wxBitmap{bitmapSize, bitmapSize});
}

[[nodiscard]] const wxBitmap &FieldBitmaps::getBackground(const BaseState &guiState) const {
  static_assert(getNumericValue(BaseState::Opened) + 1 == std::tuple_size_v<decltype(backgrounds)>,
                "Number of background bitmaps doesn't match the count of enum values");
  switch (guiState) {
  case BaseState::Closed:
  case BaseState::Hovered:
  case BaseState::Boomed:
  case BaseState::Opened:
    return backgrounds[getNumericValue(guiState)];
  }
  throw std::runtime_error(fmt::format("Invalid GUI state {}!", getNumericValue(guiState)));
}

[[nodiscard]] wxBitmap &FieldBitmaps::getBackground(const BaseState &guiState) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return const_cast<wxBitmap &>(const_cast<const FieldBitmaps *>(this)->getBackground(guiState));
}

[[nodiscard]] const wxBitmap &FieldBitmaps::getFigure(const FigureType &fieldType) const {
  static_assert(getNumericValue(FigureType::Flag) + 1 == std::tuple_size_v<decltype(figures)>,
                "Number of figure bitmaps doesn't match the count of enum values");
  switch (fieldType) {
  case FigureType::Empty:
  case FigureType::One:
  case FigureType::Two:
  case FigureType::Three:
  case FigureType::Four:
  case FigureType::Five:
  case FigureType::Six:
  case FigureType::Seven:
  case FigureType::Eight:
  case FigureType::Mine:
  case FigureType::WrongMine:
  case FigureType::Flag:
    return figures[getNumericValue(fieldType)];
  }
  throw std::runtime_error(fmt::format("Invalid field type {}!", getNumericValue(fieldType)));
}

[[nodiscard]] wxBitmap &FieldBitmaps::getFigure(const FigureType &fieldType) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  return const_cast<wxBitmap &>(const_cast<const FieldBitmaps *>(this)->getFigure(fieldType));
}

[[nodiscard]] int FieldBitmaps::getSize() const {
  return backgrounds[0].GetHeight();
}
} // namespace minesweeper_gui
