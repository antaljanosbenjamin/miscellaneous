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
  void CreateFields();
  void DestroyFields();
  void OnHello(wxCommandEvent & /*unused*/);
  void OnExit(wxCommandEvent & /*unused*/);
  void OnAbout(wxCommandEvent & /*unused*/);

  static constexpr auto defaultGridHeight = 3;
  static constexpr auto defaultGridWidth = defaultGridHeight;

  FieldBitmaps bitmaps;
  FieldPanels panels{};
  wxPanel *fieldHolderPanel{nullptr};
  minesweeper::Minesweeper game;
};
} // namespace minesweeper_gui
