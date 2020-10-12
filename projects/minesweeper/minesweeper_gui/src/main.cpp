#include <functional>
#include <memory>

#include <fmt/core.h>
#include <nonstd/span.hpp>
#include "Minesweeper.hpp"
#include "WxWidgetsWrapper.hpp"

class MyApp : public wxApp {
public:
  virtual bool OnInit();
};

enum FieldGuiState : uint8_t {
  Closed = 0,
  Flagged,
  Hoovered,
  OpenedAndBoomed,
  Opened,
};

template <typename TTo, typename TFrom>
constexpr TTo safeCast(const TFrom &from) {
  return static_cast<TTo>(from);
}

template <typename TEnum>
constexpr std::underlying_type_t<TEnum> getNumericValue(const TEnum enumValue) {
  return static_cast<std::underlying_type_t<TEnum>>(enumValue);
}

class FieldBitmaps {
public:
  FieldBitmaps(int bitmapSize) {
    for (auto &bitmap: bitmaps) {
      bitmap = wxBitmap{bitmapSize, bitmapSize};
    }
  }

  wxBitmap &getBitmap(const FieldGuiState &guiState, const minesweeper::FieldType &fieldType) {
    switch (guiState) {
    case FieldGuiState::Closed:
    case FieldGuiState::Flagged:
    case FieldGuiState::Hoovered:
    case FieldGuiState::OpenedAndBoomed:
      return bitmaps[getNumericValue(guiState)];
    case FieldGuiState::Opened:
      return getOpenedBitmap(fieldType);
    }
    throw std::runtime_error(fmt::format("Invalid GUI state {}!", getNumericValue(guiState)));
  }

  const wxBitmap &getBitmap(const FieldGuiState &guiState, const minesweeper::FieldType &fieldType) const {
    return const_cast<FieldBitmaps *>(this)->getBitmap(guiState, fieldType);
  }

  std::array<wxBitmap, 15> bitmaps;

private:
  wxBitmap &getOpenedBitmap(const minesweeper::FieldType &fieldType) {
    switch (fieldType) {
    case minesweeper::FieldType::Empty:
    case minesweeper::FieldType::One:
    case minesweeper::FieldType::Two:
    case minesweeper::FieldType::Three:
    case minesweeper::FieldType::Four:
    case minesweeper::FieldType::Five:
    case minesweeper::FieldType::Six:
    case minesweeper::FieldType::Seven:
    case minesweeper::FieldType::Eight:
    case minesweeper::FieldType::Nine:
    case minesweeper::FieldType::Mine:
      return bitmaps[getNumericValue(FieldGuiState::Opened) + getNumericValue(fieldType)];
    }
    throw std::runtime_error(fmt::format("Invalid field type {}!", getNumericValue(fieldType)));
  }
};

class FieldState {
public:
  FieldState(const uint64_t &row, const uint64_t &column)
    : fieldInfo{row, column, defaultFieldType} {
  }

  const wxBitmap &getBitmap(const FieldBitmaps &bitmaps) const {
    return bitmaps.getBitmap(guiState, fieldInfo.type);
  }

  void setStateFromGuiState(const FieldGuiState &newGuiState) {
    guiState = newGuiState;
    fieldInfo.type = defaultFieldType;
  }

  void setStateFromFieldType(const minesweeper::FieldType &fieldType) {
    guiState = FieldGuiState::Opened;
    fieldInfo.type = fieldType;
  }

private:
  static constexpr auto defaultFieldType = minesweeper::FieldType::Empty;
  FieldGuiState guiState{FieldGuiState::Closed};
  minesweeper::FieldInfo fieldInfo{};
};

class FieldsFrame;

class FieldPanel : public wxPanel {
public:
  FieldPanel(wxPanel &parent, const uint64_t x, const uint64_t y, const FieldBitmaps &bitmaps);

  DECLARE_EVENT_TABLE()

private:
  FieldsFrame &frame();
  void Render(wxDC &dc);

  void PaintEvent(wxPaintEvent &);
  void RightClickEvent(wxMouseEvent &);
  void LeftClickEvent(wxMouseEvent &);
  void MouseEnterEvent(wxMouseEvent &event);
  void MouseLeaveEvent(wxMouseEvent &event);

  FieldState state;
  const FieldBitmaps &bitmaps;
};

BEGIN_EVENT_TABLE(FieldPanel, wxPanel)
EVT_PAINT(FieldPanel::PaintEvent)
EVT_RIGHT_DOWN(FieldPanel::RightClickEvent)
EVT_LEFT_UP(FieldPanel::LeftClickEvent)
EVT_LEFT_DOWN(FieldPanel::MouseEnterEvent)
EVT_ENTER_WINDOW(FieldPanel::MouseEnterEvent)
EVT_LEAVE_WINDOW(FieldPanel::MouseLeaveEvent)
END_EVENT_TABLE()

class FieldsFrame : public wxFrame {
public:
  FieldsFrame();

private:
  static constexpr auto bitmapSize = 40;
  FieldBitmaps bitmaps{bitmapSize};
  std::vector<std::reference_wrapper<FieldPanel>> panels{};

  void CreateFields(const uint64_t width, const uint64_t height);
  void DestroyFields();
  void OnHello(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  void OnButtonClicked(wxCommandEvent &event);

  wxPanel *fieldHolderPanel;
};

enum {
  ID_Hello = 1,
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
  auto *frame = new FieldsFrame();
  frame->Show(true);
  return true;
}

FieldPanel::FieldPanel(wxPanel &parent, const uint64_t row, const uint64_t column, const FieldBitmaps &bitmaps)
  : wxPanel(&parent)
  , state{row, column}
  , bitmaps{bitmaps} {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void FieldPanel::PaintEvent(wxPaintEvent &) {
  wxBufferedPaintDC dc(this);
  Render(dc);
}

void FieldPanel::RightClickEvent(wxMouseEvent &) {
  state.setStateFromGuiState(FieldGuiState::Flagged);
  Refresh();
}

void FieldPanel::LeftClickEvent(wxMouseEvent &) {
  state.setStateFromGuiState(FieldGuiState::Opened);
  Refresh();
}

void FieldPanel::MouseEnterEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.setStateFromGuiState(FieldGuiState::Hoovered);
  Refresh();
}

void FieldPanel::MouseLeaveEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.setStateFromGuiState(FieldGuiState::Closed);
  Refresh();
}

FieldsFrame &FieldPanel::frame() {
  return *dynamic_cast<FieldsFrame *>(GetParent());
}

void FieldPanel::Render(wxDC &dc) {
  dc.DrawBitmap(state.getBitmap(bitmaps), 0, 0, false);
}

FieldsFrame::FieldsFrame()
  : wxFrame(NULL, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
  , fieldHolderPanel{new wxPanel(this)} {

  bitmaps = FieldBitmaps{40};
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");
  Bind(wxEVT_MENU, &FieldsFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &FieldsFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &FieldsFrame::OnExit, this, wxID_EXIT);

  wxMemoryDC dc;
  auto paint = [&dc](wxBitmap &bitmap, const wxColor &color) {
    const auto size = bitmap.GetSize();
    assert(size.x > 1);
    assert(size.y > 1);
    dc.SelectObject(bitmap);
    dc.SetPen(color);
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(wxBrush{color});
    constexpr auto borderSize = 5;
    dc.DrawRectangle(borderSize, borderSize, size.x - 2 * borderSize, size.y - 2 * borderSize);
    dc.SelectObject(wxNullBitmap);
  };

  constexpr auto dontCareFieldType = minesweeper::FieldType::Empty;
  paint(bitmaps.getBitmap(FieldGuiState::Closed, dontCareFieldType), wxColor{50, 50, 170});
  paint(bitmaps.getBitmap(FieldGuiState::Flagged, dontCareFieldType), *wxCYAN);
  paint(bitmaps.getBitmap(FieldGuiState::OpenedAndBoomed, dontCareFieldType), *wxRED);
  paint(bitmaps.getBitmap(FieldGuiState::Hoovered, dontCareFieldType), wxColor{180, 150, 0});
  paint(bitmaps.getBitmap(FieldGuiState::Opened, minesweeper::FieldType::Empty), *wxLIGHT_GREY);

  auto *topSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(fieldHolderPanel);
  SetSizerAndFit(topSizer);

  CreateFields(10, 10);
}

void FieldsFrame::CreateFields(const uint64_t width, const uint64_t height) {
  fieldHolderPanel->Hide();
  auto *gridSizer = new wxGridSizer(safeCast<int>(height), safeCast<int>(width), 0, 0);

  const auto fieldCount = width * height;

  auto fieldSize = wxSize(bitmapSize, bitmapSize);
  panels.reserve(fieldCount);
  for (uint64_t fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {

    auto fieldPanel = new FieldPanel(*fieldHolderPanel, fieldIndex / width, fieldIndex % width, bitmaps);

    fieldPanel->SetSize(fieldSize);
    fieldPanel->SetMinSize(fieldSize);
    fieldPanel->SetMaxSize(fieldSize);
    fieldPanel->SetWindowStyle(wxBORDER_NONE | wxBU_EXACTFIT);
    auto panelRef = std::ref(*fieldPanel);
    panels.push_back(std::move(panelRef));
    gridSizer->Add(fieldPanel, wxALL, 1);
  }

  fieldHolderPanel->SetSizer(gridSizer);
  gridSizer->Fit(fieldHolderPanel);
  gridSizer->SetSizeHints(fieldHolderPanel);
  fieldHolderPanel->Show(true);
  GetSizer()->Fit(this);
}

void FieldsFrame::DestroyFields() {
  fieldHolderPanel->SetSizer(nullptr);
  panels.clear();
  if (fieldHolderPanel->DestroyChildren() != true) {
    throw std::runtime_error("Something went wrong");
  }
}

void FieldsFrame::OnExit(wxCommandEvent &) {
  Close(true);
}

void FieldsFrame::OnAbout(wxCommandEvent &) {
  wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
  CreateFields(20, 20);
  Refresh();
}

void FieldsFrame::OnHello(wxCommandEvent &) {
  wxLogMessage("Hello world from wxWidgets!");
  DestroyFields();
}
