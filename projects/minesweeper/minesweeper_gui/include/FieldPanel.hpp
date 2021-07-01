#pragma once

#include <vector>

#include <Minesweeper.hpp>
#include "FieldBitmaps.hpp"
#include "FieldState.hpp"
#include "WxWidgetsWrapper.hpp"
#include "utils/PropagateConst.hpp"

namespace minesweeper_gui {
class FieldPanel;

using FieldPanels = std::vector<std::reference_wrapper<FieldPanel>>;

class FieldPanel : public wxPanel {
public:
  FieldPanel(wxPanel &parent, const uint64_t row, const uint64_t column, const FieldBitmaps &bitmaps,
             FieldPanels &panels, minesweeper::Minesweeper &game);

  DECLARE_EVENT_TABLE() // NOLINT(modernize-avoid-c-arrays,cppcoreguidelines-avoid-non-const-global-variables)

private:
  void Render(wxDC &dc);

  void PaintEvent(wxPaintEvent & /*unused*/);
  void RightClickEvent(wxMouseEvent & /*unused*/);
  void LeftClickEvent(wxMouseEvent & /*unused*/);
  void MouseEnterEvent(wxMouseEvent &event);
  void MouseLeaveEvent(wxMouseEvent &event);

  FieldState state;
  const FieldBitmaps *bitmaps;
  utils::PropagateConst<FieldPanels *> panels;
  utils::PropagateConst<minesweeper::Minesweeper *> game;
};

} // namespace minesweeper_gui
