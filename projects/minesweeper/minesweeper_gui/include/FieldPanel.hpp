#pragma once

#include <vector>

#include <Minesweeper.hpp>
#include "FieldBitmaps.hpp"
#include "FieldState.hpp"
#include "MouseStateHandler.hpp"
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
  void render(wxDC &dc);

  void paintEvent(wxPaintEvent & /*unused*/);
  void rightClickEvent(wxMouseEvent & /*unused*/);
  void leftClickEvent(wxMouseEvent & /*unused*/);
  void mouseEnterEvent(wxMouseEvent &event);
  void mouseLeaveEvent(wxMouseEvent &event);
  void handleMouseAction(const MouseStateHandler::Action action);
  void processOpenInfo(const minesweeper::OpenInfo openInfo);

  FieldState state;
  const FieldBitmaps *bitmaps;
  utils::PropagateConst<FieldPanels *> panels;
  utils::PropagateConst<minesweeper::Minesweeper *> game;
};

} // namespace minesweeper_gui
