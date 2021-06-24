#pragma once

#include <Minesweeper.hpp>
#include "FieldBitmaps.hpp"
#include "FieldPanel.hpp"
#include "WxWidgetsWrapper.hpp"

namespace minesweeper_gui {

class FieldsFrame : public wxFrame {
public:
  FieldsFrame();

  static FieldPanel &getFieldPanel(FieldPanels &panels, uint64_t row, uint64_t col,
                                   const minesweeper::Minesweeper::Dimension &size);

private:
  static FieldBitmaps getDefaultBitmaps();
  void createFields();
  void onNewGame(wxCommandEvent & /*unused*/);
  void onExit(wxCommandEvent & /*unused*/);
  void onAbout(wxCommandEvent & /*unused*/);

  static constexpr auto defaultGridHeight = 3;
  static constexpr auto defaultGridWidth = defaultGridHeight;

  FieldBitmaps bitmaps{getDefaultBitmaps()};
  FieldPanels panels{};
  std::experimental::propagate_const<wxPanel *> fieldHolderPanel{nullptr};
  minesweeper::GameLevel gameLevel{minesweeper::GameLevel::Beginner};
  minesweeper::Minesweeper game;
};
} // namespace minesweeper_gui
