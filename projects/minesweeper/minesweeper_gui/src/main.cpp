#include <array>
#include <functional>
#include <map>
#include <memory>
#include <utility>

#include <fmt/core.h>
#include <nonstd/span.hpp>
#include "Minesweeper.hpp"
#include "Resources.hpp"
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
  Mine,
  Flag,
};

constexpr auto figureTypeCount = 11;

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
    std::fill(backgrounds.begin(), backgrounds.end(),
              wxBitmap{bitmapSize, bitmapSize}); // NOLINT(clang-analyzer-optin.cplusplus.VirtualCall)
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
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
    case FigureType::Mine:
    case FigureType::Flag:
      return figures[getNumericValue(fieldType)];
    }
    throw std::runtime_error(fmt::format("Invalid field type {}!", getNumericValue(fieldType)));
  }

  [[nodiscard]] wxBitmap &getFigure(const FigureType &fieldType) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
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
    figureType = static_cast<FigureType>((row + column) % (static_cast<uint64_t>(FigureType::Flag) + 1));
    if (FigureType::Mine == figureType && row % 2 == 0) {
      baseState = BaseState::Boomed;
    }
  }

  void updateWithBaseState(const BaseState &newBaseState) {
    if (newBaseState == BaseState::Hoovered && !canBeHoover()) {
      return;
    }
    if (newBaseState == BaseState::Closed && !isHoovered()) {
      return;
    }
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
  [[nodiscard]] bool canBeHoover() const {
    return baseState == BaseState::Closed;
  }

  [[nodiscard]] bool isHoovered() const {
    return baseState == BaseState::Hoovered;
  }

  [[nodiscard]] bool shouldDrawFigure() const {
    return baseState == BaseState::Boomed || baseState == BaseState::Opened || figureType == FigureType::Flag;
  }

  [[nodiscard]] const wxBitmap &getBackground(const FieldBitmaps &bitmaps) const {
    return bitmaps.getBackground(baseState);
  }

  [[nodiscard]] const wxBitmap &getFigure(const FieldBitmaps &bitmaps) const {
    return bitmaps.getFigure(figureType);
  }

  static constexpr auto defaultFigureType = FigureType::Empty;
  BaseState baseState{BaseState::Closed};
  FigureType figureType{defaultFigureType};
  CLANG_MAYBE_UNUSED uint64_t row{0U};    // NOLINT(clang-diagnostic-unused-private-field)
  CLANG_MAYBE_UNUSED uint64_t column{0U}; // NOLINT(clang-diagnostic-unused-private-field)
};

class FieldsFrame;

class FieldPanel : public wxPanel {
public:
  FieldPanel(wxPanel &parent, const uint64_t row, const uint64_t column, const FieldBitmaps &bitmaps);

  // cppcheck-suppress unknownMacro
  DECLARE_EVENT_TABLE() // NOLINT(modernize-avoid-c-arrays)

private:
  FieldsFrame &frame();
  void Render(wxDC &dc);

  void PaintEvent(wxPaintEvent & /*unused*/);
  void RightClickEvent(wxMouseEvent & /*unused*/);
  void LeftClickEvent(wxMouseEvent & /*unused*/);
  void MouseEnterEvent(wxMouseEvent &event);
  void MouseLeaveEvent(wxMouseEvent &event);

  FieldState state;
  const FieldBitmaps &bitmaps;
};

BEGIN_EVENT_TABLE(FieldPanel, wxPanel) // NOLINT(cert-err58-cpp, modernize-avoid-c-arrays)
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
  static constexpr auto bitmapSize = 25;
  FieldBitmaps bitmaps{bitmapSize};
  std::vector<std::reference_wrapper<FieldPanel>> panels{};

  void CreateFields(const uint64_t width, const uint64_t height);
  void DestroyFields();
  void OnHello(wxCommandEvent & /*unused*/);
  void OnExit(wxCommandEvent & /*unused*/);
  void OnAbout(wxCommandEvent & /*unused*/);

  static constexpr auto defaultGridHeight = 10;
  static constexpr auto defaultGridWidth = defaultGridHeight;

  wxPanel *fieldHolderPanel{nullptr};
};

enum {
  ID_Hello = 1,
};

wxIMPLEMENT_APP(MyApp); // NOLINT(cert-err58-cpp, cppcoreguidelines-pro-type-static-cast-downcast)

bool MyApp::OnInit() {
  wxImage::AddHandler(new wxPNGHandler{}); // NOLINT(cppcoreguidelines-owning-memory)
  auto frame = std::make_unique<FieldsFrame>();
  frame->Show(true);
  static_cast<void>(frame.release());
  return true;
}

FieldPanel::FieldPanel(wxPanel &parent, const uint64_t row, const uint64_t column, const FieldBitmaps &bitmaps)
  : wxPanel(&parent)
  , state{row, column}
  , bitmaps{bitmaps} {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void FieldPanel::PaintEvent(wxPaintEvent & /*unused*/) {
  wxBufferedPaintDC dc(this);
  Render(dc);
}

void FieldPanel::RightClickEvent(wxMouseEvent & /*unused*/) {
  state.updateWithFigureType(FigureType::Flag);
  Refresh();
}

void FieldPanel::LeftClickEvent(wxMouseEvent & /*unused*/) {
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
  : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER) { // NOLINT(hicpp-signed-bitwise)

  auto menuFile = std::make_unique<wxMenu>();
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  auto menuHelp = std::make_unique<wxMenu>();
  menuHelp->Append(wxID_ABOUT);
  auto menuBar = std::make_unique<wxMenuBar>();
  menuBar->Append(menuFile.get(), "&File");
  static_cast<void>(menuFile.release());
  menuBar->Append(menuHelp.get(), "&Help");
  static_cast<void>(menuHelp.release());
  SetMenuBar(menuBar.get());
  static_cast<void>(menuBar.release());
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");
  Bind(wxEVT_MENU, &FieldsFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &FieldsFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &FieldsFrame::OnExit, this, wxID_EXIT);

  auto fs = cmrc::minesweeper_gui_resources::get_filesystem();

  {
    const auto backgroundImageBindings = std::map<BaseState, std::string>{
        {BaseState::Closed, "closed.png"},
        {BaseState::Boomed, "boomed.png"},
        {BaseState::Hoovered, "hoovered.png"},
        {BaseState::Opened, "opened.png"},
    };

    for (const auto &binding: backgroundImageBindings) {
      auto image = fs.open(binding.second);
      auto streamToReadFrom = wxMemoryInputStream(image.begin(), image.size());
      bitmaps.getBackground(binding.first) = wxBitmap(wxImage(streamToReadFrom));
    }
  }

  {
    const auto figureImageBindings = std::map<FigureType, std::string>{
        {FigureType::Empty, "empty.png"}, {FigureType::One, "one.png"},     {FigureType::Two, "two.png"},
        {FigureType::Three, "three.png"}, {FigureType::Four, "four.png"},   {FigureType::Five, "five.png"},
        {FigureType::Six, "six.png"},     {FigureType::Seven, "seven.png"}, {FigureType::Eight, "eight.png"},
        {FigureType::Mine, "mine.png"},   {FigureType::Flag, "flag.png"},
    };

    for (const auto &binding: figureImageBindings) {
      auto image = fs.open(binding.second);
      auto streamToReadFrom = wxMemoryInputStream(image.begin(), image.size());
      bitmaps.getFigure(binding.first) = wxBitmap(wxImage(streamToReadFrom));
    }
  }

  std::unique_ptr<wxBoxSizer> topSizer{new wxBoxSizer(wxVERTICAL)};
  auto fieldHolderPanelOwner = std::make_unique<wxPanel>(this);
  topSizer->Add(fieldHolderPanelOwner.get());
  fieldHolderPanel = fieldHolderPanelOwner.release();
  SetSizerAndFit(topSizer.get());
  static_cast<void>(topSizer.release());

  CreateFields(defaultGridWidth, defaultGridHeight);
}

void FieldsFrame::CreateFields(const uint64_t width, const uint64_t height) {
  fieldHolderPanel->Hide();
  auto gridSizerOwner = std::make_unique<wxGridSizer>(safeCast<int>(height), safeCast<int>(width), 0, 0);

  const auto fieldCount = width * height;

  auto fieldSize = wxSize(bitmapSize, bitmapSize);
  panels.reserve(fieldCount);
  for (uint64_t fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {

    auto fieldPanel = std::make_unique<FieldPanel>(*fieldHolderPanel, fieldIndex / width, fieldIndex % width, bitmaps);

    fieldPanel->SetSize(fieldSize);
    fieldPanel->SetMinSize(fieldSize);
    fieldPanel->SetMaxSize(fieldSize);
    fieldPanel->SetWindowStyle(wxBORDER_NONE | wxBU_EXACTFIT); // NOLINT(hicpp-signed-bitwise)
    auto panelRef = std::ref(*fieldPanel);
    panels.push_back((panelRef));
    gridSizerOwner->Add(fieldPanel.get(), wxALL, 1);
    static_cast<void>(fieldPanel.release());
  }

  auto *gridSizer = gridSizerOwner.get();
  fieldHolderPanel->SetSizer(gridSizer);
  static_cast<void>(gridSizerOwner.release());
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

void FieldsFrame::OnExit(wxCommandEvent & /*unused*/) {
  Close(true);
}

void FieldsFrame::OnAbout(wxCommandEvent & /*unused*/) {
  wxMessageBox("This is a wxWidgets Hello World example", "About Hello World",
               wxOK | wxICON_INFORMATION); // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
  CreateFields(2 * defaultGridWidth, 2 * defaultGridWidth);
  Refresh();
}

void FieldsFrame::OnHello(wxCommandEvent & /*unused*/) {
  wxLogMessage("Hello world from wxWidgets!");
  DestroyFields();
}
