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
    , clickedMine{bitmapSize, bitmapSize} {
  }

  wxBitmap original;
  wxBitmap empty;
  wxBitmap flagged;
  wxBitmap mine;
  wxBitmap clickedMine;
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
  const wxBitmap &getBitmap(const FieldBitmaps &bitmaps) {
    switch (baseState) {
    case BaseState::Closed:
      return bitmaps.original;
    case BaseState::Flagged:
      return bitmaps.flagged;
    case BaseState::Opened:
      return getOpenedBitmap(bitmaps);
    }
    throw std::runtime_error(fmt::format("Invalid base state {}!", getNumericValue(baseState)));
  }

private:
  enum BaseState {
    Closed,
    Opened,
    Flagged,
  };

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

  BaseState baseState{BaseState::Closed};
  minesweeper::FieldInfo fieldInfo{};
};

class MineButton : public wxButton {
public:
  using wxButton::wxButton;

  void SetFieldBitmaps(const FieldBitmaps newBitmaps) {
    bitmaps = &newBitmaps;
    SetBitmap(state.getBitmap(newBitmaps));
  }

private:
  FieldState state{};
  const FieldBitmaps *bitmaps{nullptr};
};

class MyFrame : public wxFrame {
public:
  MyFrame();

private:
  FieldBitmaps bitmaps{100};
  std::vector<MineButton *> buttonPtrs{};

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
  bitmaps = FieldBitmaps{35};
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
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SetBrush(brush);
    dc.DrawRectangle(1, 1, size.x - 1, size.y - 1);
    dc.SelectObject(wxNullBitmap);
  };

  paint(bitmaps.original, *wxBLUE_BRUSH);
  paint(bitmaps.empty, *wxGREY_BRUSH);
  paint(bitmaps.flagged, *wxCYAN_BRUSH);
  paint(bitmaps.mine, *wxYELLOW_BRUSH);
  paint(bitmaps.clickedMine, *wxRED_BRUSH);

  CreateFields(10, 10);
}

void MyFrame::CreateFields(const uint64_t width, const uint64_t height) {
  auto *gridSizer = new wxGridSizer(safeCast<int>(height), safeCast<int>(width), 0, 0);
  this->SetSizer(gridSizer);

  const auto buttonCount = safeCast<int>(width * height);

  const auto buttonSize = bitmaps.empty.GetSize();
  buttonPtrs.reserve(buttonCount);

  for (auto buttonId = 0; buttonId < buttonCount; ++buttonId) {

    auto button = std::make_unique<MineButton>(this, wxID_ANY);
    button->SetSize(buttonSize);
    button->SetWindowStyle(wxBORDER_NONE | wxBU_EXACTFIT);
    button->SetFieldBitmaps(bitmaps);
    buttonPtrs.push_back(button.get());
    auto *buttonPtr = button.get();
    gridSizer->Add(button.release(), wxALL, 1);
    buttonPtr->Bind(
        wxEVT_COMMAND_BUTTON_CLICKED,
        [this, gridSizer](wxCommandEvent &event) {
          [[maybe_unused]] auto *eventObject = event.GetEventObject();
          auto *button = dynamic_cast<wxButton *>(event.GetEventObject());
          if (nullptr == button) {
            return;
          }
          button->SetBitmap(bitmaps.empty);
        },
        wxID_ANY);
    buttonPtr->Bind(
        wxEVT_RIGHT_DOWN,
        [this, gridSizer](wxMouseEvent &event) {
          [[maybe_unused]] auto *eventObject = event.GetEventObject();
          auto *button = dynamic_cast<wxButton *>(event.GetEventObject());
          if (nullptr == button) {
            return;
          }
          button->SetBitmap(bitmaps.flagged);
        },
        wxID_ANY);
  }

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
