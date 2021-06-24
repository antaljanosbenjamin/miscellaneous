#include "FieldPanel.hpp"

#include "FieldsFrame.hpp"

namespace minesweeper_gui {

FieldPanel::FieldPanel(wxPanel &parent, const uint64_t row, const uint64_t column, const FieldBitmaps &bitmaps,
                       FieldPanels &panels, minesweeper::Minesweeper &game)
  : wxPanel(&parent)
  , state{row, column}
  , bitmaps{&bitmaps}
  , panels{&panels}
  , game{&game} {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void FieldPanel::paintEvent(wxPaintEvent & /*unused*/) {
  wxBufferedPaintDC dc(this);
  render(dc);
}

void FieldPanel::rightClickEvent(wxMouseEvent & /*unused*/) {
  this->state.toggleFlag(*game);
  Refresh();
}

void FieldPanel::leftClickEvent(wxMouseEvent & /*unused*/) {
  auto newlyOpenedFields = this->state.open(*game);
  for (const auto &newlyOpenedField: newlyOpenedFields) {
    auto &fieldPanel =
        FieldsFrame::getFieldPanel(*panels, static_cast<uint64_t>(newlyOpenedField.row),
                                   static_cast<uint64_t>(newlyOpenedField.column), game->getSize().value());
    fieldPanel.state.updateWithBaseState(BaseState::Opened);
    fieldPanel.state.updateWithFigureType(static_cast<FigureType>(newlyOpenedField.type));
    fieldPanel.Refresh();
  }
}

void FieldPanel::mouseEnterEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Hoovered);
  Refresh();
}

void FieldPanel::mouseLeaveEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Closed);
  Refresh();
}

void FieldPanel::render(wxDC &dc) {
  state.draw(dc, *bitmaps);
}
// NOLINTNEXTLINE(modernize-avoid-c-arrays,cppcoreguidelines-avoid-non-const-global-variables)
BEGIN_EVENT_TABLE(FieldPanel, wxPanel)
EVT_PAINT(FieldPanel::paintEvent)
EVT_LEFT_UP(FieldPanel::leftClickEvent)
EVT_RIGHT_DOWN(FieldPanel::rightClickEvent)
EVT_RIGHT_DCLICK(FieldPanel::rightClickEvent)
EVT_ENTER_WINDOW(FieldPanel::mouseEnterEvent)
EVT_LEAVE_WINDOW(FieldPanel::mouseLeaveEvent)
END_EVENT_TABLE()

} // namespace minesweeper_gui
