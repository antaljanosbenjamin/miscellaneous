#include "FieldPanel.hpp"

#include <stdexcept>

#include <utils/Likely.hpp>
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
  this->processOpenInfo(this->state.open(*game));
}

void FieldPanel::mouseEnterEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Hovered);
  Refresh();
}

void FieldPanel::mouseLeaveEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Closed);
  Refresh();
}

void FieldPanel::handleMouseAction(const MouseStateHandler::Action action) {
  using Action = MouseStateHandler::Action;
  switch (action) {
  case Action::Hover:
    state.updateWithBaseState(BaseState::Hovered);
    return;
  case Action::Unhover:
    state.updateWithBaseState(BaseState::Closed);
    return;
  case Action::Open:
    this->processOpenInfo(this->state.open(*game));
    return;
  case Action::Flag:
    this->state.toggleFlag(*game);
    Refresh();
  case Action::HoverNeighbors:
  case Action::UnhoverNeighbors:
  case Action::OpenNeighbors:
  case Action::None:
    return;
  }
}

void FieldPanel::processOpenInfo(const minesweeper::OpenInfo openInfo) {
  switch (openInfo.openResult) {
  case minesweeper::OpenResult::Ok:
  case minesweeper::OpenResult::Winner: {
    for (const auto &newlyOpenedField: openInfo.newlyOpenedFields) {
      auto &fieldPanel =
          FieldsFrame::getFieldPanel(*panels, static_cast<uint64_t>(newlyOpenedField.row),
                                     static_cast<uint64_t>(newlyOpenedField.column), game->getSize().value());
      fieldPanel.state.updateWithBaseState(BaseState::Opened);
      fieldPanel.state.updateWithFigureType(static_cast<FigureType>(newlyOpenedField.type));
      fieldPanel.Refresh();
    }
    break;
  }
  case minesweeper::OpenResult::IsFlagged: {
    if (MY_UNLIKELY(!openInfo.newlyOpenedFields.empty())) {
      throw std::runtime_error("Unexpected newly opened fields!");
    }
    break;
  }
  case minesweeper::OpenResult::Boom: {
    for (const auto &newlyOpenedField: openInfo.newlyOpenedFields) {
      if (newlyOpenedField.type == minesweeper::FieldType::Mine) {
        auto &fieldPanel =
            FieldsFrame::getFieldPanel(*panels, static_cast<uint64_t>(newlyOpenedField.row),
                                       static_cast<uint64_t>(newlyOpenedField.column), game->getSize().value());

        if (&fieldPanel == this) {
          fieldPanel.state.updateWithBaseState(BaseState::Boomed);
        } else {
          fieldPanel.state.updateWithBaseState(BaseState::Opened);
        }
        fieldPanel.state.updateWithFigureType(static_cast<FigureType>(newlyOpenedField.type));
        fieldPanel.Refresh();
      }
    }
    break;
  }
  }
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
