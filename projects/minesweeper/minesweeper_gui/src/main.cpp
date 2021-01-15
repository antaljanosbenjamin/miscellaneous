#include <array>
#include <functional>
#include <memory>

#include <fmt/core.h>
#include <nonstd/span.hpp>
#include "Minesweeper.hpp"
#include "WxWidgetsWrapper.hpp"

class MyApp : public wxApp {
public:
  bool OnInit() override;
};

enum class BaseState : uint8_t {
  Closed = 0,
  Hoovered,
  Boomed,
  Opened,
};

constexpr auto baseStateCount = 4;

enum class FigureType : uint8_t {
  Empty = 0,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Mine,
  Flag,
};

constexpr auto figureTypeCount = 12;

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
  explicit FieldBitmaps(int bitmapSize) {
    std::fill(backgrounds.begin(), backgrounds.end(), wxBitmap{bitmapSize, bitmapSize});
    std::fill(figures.begin(), figures.end(), wxBitmap{bitmapSize, bitmapSize});
  }

  [[nodiscard]] const wxBitmap &getBackground(const BaseState &guiState) const {
    static_assert(getNumericValue(BaseState::Opened) + 1 == std::tuple_size_v<decltype(backgrounds)>,
                  "Number of background bitmaps doesn't match the count of enum values");
    switch (guiState) {
    case BaseState::Closed:
    case BaseState::Hoovered:
    case BaseState::Boomed:
    case BaseState::Opened:
      return backgrounds[getNumericValue(guiState)];
    }
    throw std::runtime_error(fmt::format("Invalid GUI state {}!", getNumericValue(guiState)));
  }

  [[nodiscard]] wxBitmap &getBackground(const BaseState &guiState) {
    // NOTLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<wxBitmap &>(const_cast<const FieldBitmaps *>(this)->getBackground(guiState));
  }

  [[nodiscard]] const wxBitmap &getFigure(const FigureType &fieldType) const {
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
    case FigureType::Nine:
    case FigureType::Mine:
    case FigureType::Flag:
      return figures[getNumericValue(fieldType)];
    }
    throw std::runtime_error(fmt::format("Invalid field type {}!", getNumericValue(fieldType)));
  }

  [[nodiscard]] wxBitmap &getFigure(const FigureType &fieldType) {
    // NOTLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<wxBitmap &>(const_cast<const FieldBitmaps *>(this)->getFigure(fieldType));
  }

  std::array<wxBitmap, baseStateCount> backgrounds;
  std::array<wxBitmap, figureTypeCount> figures;
};

class FieldState {
public:
  FieldState(const uint64_t &row, const uint64_t &column)
    : row{row}
    , column{column} {
  }

  void updateWithBaseState(const BaseState &newBaseState) {
    baseState = newBaseState;
  }

  void updateWithFigureType(const FigureType &newFigureType) {
    if (newFigureType == FigureType::Flag && baseState != BaseState::Closed) {
      return;
    }

    figureType = newFigureType;
  }

  void draw(wxDC &dc, const FieldBitmaps &bitmaps) const {
    dc.DrawBitmap(getBackground(bitmaps), 0, 0, false);
    if (shouldDrawFigure()) {
      dc.DrawBitmap(getFigure(bitmaps), 0, 0, false);
    }
  }

private:
  bool shouldDrawFigure() const {
    return baseState == BaseState::Boomed || baseState == BaseState::Opened || figureType == FigureType::Flag;
  }

  const wxBitmap &getBackground(const FieldBitmaps &bitmaps) const {
    return bitmaps.getBackground(baseState);
  }

  const wxBitmap &getFigure(const FieldBitmaps &bitmaps) const {
    return bitmaps.getFigure(figureType);
  }

  static constexpr auto defaultFigureType = FigureType::Empty;
  BaseState baseState{BaseState::Closed};
  FigureType figureType{defaultFigureType};

  PRAGMA_WARNING_PUSH_GCC();
  PRAGMA_WARNING_IGNORE_GCC(WARNING_IGNORED_ATTRIBUTES_GCC);
  [[maybe_unused]] uint64_t row{0U};
  [[maybe_unused]] uint64_t column{0U};
  PRAGMA_WARNING_POP_GCC();
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
  state.updateWithFigureType(FigureType::Flag);
  Refresh();
}

void FieldPanel::LeftClickEvent(wxMouseEvent &) {
  state.updateWithBaseState(BaseState::Opened);
  Refresh();
}

void FieldPanel::MouseEnterEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Hoovered);
  Refresh();
}

void FieldPanel::MouseLeaveEvent(wxMouseEvent &event) {
  if (!event.LeftIsDown()) {
    return;
  }
  state.updateWithBaseState(BaseState::Closed);
  Refresh();
}

FieldsFrame &FieldPanel::frame() {
  return *dynamic_cast<FieldsFrame *>(GetParent());
}

void FieldPanel::Render(wxDC &dc) {
  state.draw(dc, bitmaps);
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
    constexpr auto borderSize = 5;
    assert(size.x >= 2 * borderSize);
    assert(size.y >= 2 * borderSize);
    dc.SelectObject(bitmap);
    dc.SetPen(color);
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(wxBrush{color});
    dc.DrawRectangle(borderSize, borderSize, size.x - 2 * borderSize, size.y - 2 * borderSize);
    dc.SelectObject(wxNullBitmap);
  };

  paint(bitmaps.getBackground(BaseState::Closed), wxColor{50, 50, 170});
  paint(bitmaps.getBackground(BaseState::Boomed), *wxRED);
  paint(bitmaps.getBackground(BaseState::Hoovered), wxColor{180, 150, 0});
  paint(bitmaps.getBackground(BaseState::Opened), *wxLIGHT_GREY);

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
