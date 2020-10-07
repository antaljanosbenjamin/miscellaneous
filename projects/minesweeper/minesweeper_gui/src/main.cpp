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

struct FieldBitmaps {

  FieldBitmaps(int bitmapSize)
    : original{bitmapSize, bitmapSize}
    , empty{bitmapSize, bitmapSize}
    , flagged{bitmapSize, bitmapSize}
    , mine{bitmapSize, bitmapSize}
    , clickedMine{bitmapSize, bitmapSize}
    , hoovered{bitmapSize, bitmapSize} {
  }

  wxBitmap original;
  wxBitmap empty;
  wxBitmap flagged;
  wxBitmap mine;
  wxBitmap clickedMine;
  wxBitmap hoovered;
};

template <typename TTo, typename TFrom>
constexpr TTo safeCast(const TFrom &from) {
  return static_cast<TTo>(from);
}

template <typename TEnum>
constexpr std::underlying_type_t<TEnum> getNumericValue(const TEnum enumValue) {
  return static_cast<std::underlying_type_t<TEnum>>(enumValue);
}

class FieldState {
public:
  enum BaseState {
    Closed,
    Opened,
    Flagged,
    Hoovered,
  };

  const wxBitmap &getBitmap(const FieldBitmaps &bitmaps) {
    switch (baseState) {
    case BaseState::Closed:
      return bitmaps.original;
    case BaseState::Flagged:
      return bitmaps.flagged;
    case BaseState::Opened:
      return getOpenedBitmap(bitmaps);
    case BaseState::Hoovered:
      return bitmaps.hoovered;
    }
    throw std::runtime_error(fmt::format("Invalid base state {}!", getNumericValue(baseState)));
  }

private:
  const wxBitmap &getOpenedBitmap(const FieldBitmaps &bitmaps) {
    switch (fieldInfo.type) {
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
      return bitmaps.empty;
    case minesweeper::FieldType::Mine:
      return bitmaps.mine;
    }
    throw std::runtime_error(fmt::format("Invalid field type {}!", getNumericValue(fieldInfo.type)));
  }

  // TODO REMOVE THIS
public:
  BaseState baseState{BaseState::Closed};
  minesweeper::FieldInfo fieldInfo{};
};

class FieldPanel : public wxPanel {
public:
  FieldPanel(wxWindow *parent, const FieldBitmaps &bitmaps)
    : wxPanel(parent)
    , bitmaps{bitmaps} {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
  }

  void Render(wxDC &dc) {
    dc.DrawBitmap(state.getBitmap(bitmaps), 0, 0, false);
  }

  void PaintEvent(wxPaintEvent &) {
    wxBufferedPaintDC dc(this);
    Render(dc);
  }

  void RightClickEvent(wxMouseEvent &) {
    state.baseState = FieldState::BaseState::Flagged;
    Refresh();
    Update();
  }

  void LeftClickEvent(wxMouseEvent &) {
    state.baseState = FieldState::BaseState::Opened;
    Refresh();
    Update();
  }

  void MouseEnterEvent(wxMouseEvent &event) {
    if (!event.LeftIsDown()) {
      return;
    }
    state.baseState = FieldState::BaseState::Hoovered;
    Refresh();
    Update();
  }

  void MouseLeaveEvent(wxMouseEvent &event) {
    if (!event.LeftIsDown()) {
      return;
    }
    state.baseState = FieldState::BaseState::Closed;
    Refresh();
    Update();
  }

  DECLARE_EVENT_TABLE()

private:
  // TODO REMOVE THIS
public:
  FieldState state{};
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

class MyFrame : public wxFrame {
public:
  MyFrame();

private:
  FieldBitmaps bitmaps{100};
  std::vector<std::reference_wrapper<FieldPanel>> panels{};

  void CreateFields(const uint64_t width, const uint64_t height);
  void DestroyFields();
  void OnHello(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
  void OnButtonClicked(wxCommandEvent &event);
};

enum {
  ID_Hello = 1,
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
  auto *frame = new MyFrame();
  frame->Show(true);
  return true;
}

MyFrame::MyFrame()
  : wxFrame(NULL, wxID_ANY, "Hello World") {
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
  Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

  wxMemoryDC dc;
  auto paint = [&dc](wxBitmap &bitmap, const wxBrush &brush) {
    const auto size = bitmap.GetSize();
    assert(size.x > 1);
    assert(size.y > 1);
    dc.SelectObject(bitmap);
    dc.SetPen(*wxBLACK);
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(brush);
    constexpr auto borderSize = 5;
    dc.DrawRectangle(borderSize, borderSize, size.x - 2 * borderSize, size.y - 2 * borderSize);
    dc.SelectObject(wxNullBitmap);
  };

  paint(bitmaps.original, wxBrush{wxColor{50, 50, 170}});
  paint(bitmaps.empty, *wxLIGHT_GREY_BRUSH);
  paint(bitmaps.flagged, *wxCYAN_BRUSH);
  paint(bitmaps.mine, *wxYELLOW_BRUSH);
  paint(bitmaps.clickedMine, *wxRED_BRUSH);
  paint(bitmaps.hoovered, wxBrush{wxColor{180, 150, 0}});

  CreateFields(10, 10);
}

void MyFrame::CreateFields(const uint64_t width, const uint64_t height) {
  auto *gridSizer = new wxGridSizer(safeCast<int>(height), safeCast<int>(width), 0, 0);
  this->SetSizer(gridSizer);

  const auto fieldCount = safeCast<int>(width * height);

  const auto fieldSize = bitmaps.empty.GetSize();
  panels.reserve(fieldCount);

  for (auto fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {
    auto panel = std::make_unique<FieldPanel>(this, bitmaps);
    panel->SetSize(fieldSize);
    panel->SetMinSize(fieldSize);
    panel->SetMaxSize(fieldSize);
    panel->SetWindowStyle(wxBORDER_NONE | wxBU_EXACTFIT);
    auto panelRef = std::ref(*panel.get());
    gridSizer->Add(panel.release(), wxALL, 1);
    // panelRef.get().Bind(
    //    wxEVT_LEFT_UP,
    //    [this, gridSizer](wxMouseEvent &event) {
    //      [[maybe_unused]] auto *eventObject = event.GetEventObject();
    //      auto *panel = dynamic_cast<FieldPanel *>(event.GetEventObject());
    //      if (nullptr == panel) {
    //        return;
    //      }
    //      panel->SetState(FieldState::BaseState::Opened);
    //      panel->Refresh();
    //      panel->Update();
    //    },
    //    wxID_ANY);
    // panelRef.get().Bind(
    //    wxEVT_RIGHT_DOWN,
    //    [this, gridSizer](wxMouseEvent &event) {
    //      [[maybe_unused]] auto *eventObject = event.GetEventObject();
    //      auto *panel = dynamic_cast<FieldPanel *>(event.GetEventObject());
    //      if (nullptr == panel) {
    //        return;
    //      }
    //      panel->SetState(FieldState::BaseState::Flagged);
    //      panel->Refresh();
    //      panel->Update();
    //    },
    //    wxID_ANY);
    panels.push_back(std::move(panelRef));
  }

  panels[1].get().state.baseState = FieldState::BaseState::Flagged;
  gridSizer->Fit(this);
  gridSizer->SetSizeHints(this);
}

void MyFrame::OnExit(wxCommandEvent &) {
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &) {
  wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent &) {
  wxLogMessage("Hello world from wxWidgets!");
}
