#include "FieldsFrame.hpp"

#include <concepts>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "BaseState.hpp"
#include "FigureType.hpp"
#include "Resources.hpp"
#include "utils/SafeCast.hpp"

namespace {
enum class NewGameId : int {
  LastLevel = 1,
  Beginner = 2,
  Intermediate = 3,
  Expert = 4,
};

constexpr int toInt(NewGameId id) {
  static_assert(std::same_as<std::underlying_type_t<NewGameId>, int>);
  return static_cast<int>(id);
}

NewGameId toNewGameId(int id) {
  static_assert(std::same_as<std::underlying_type_t<NewGameId>, int>);
  constexpr auto minValue = toInt(NewGameId::LastLevel);
  constexpr auto maxValue = toInt(NewGameId::Expert);
  if (id < minValue || id > maxValue) {
    throw std::logic_error(fmt::format("Invalid NewGameId {} (minimum {}, maximum {})", id, minValue, maxValue));
  }
  return static_cast<NewGameId>(id);
}

} // namespace

namespace minesweeper_gui {
FieldsFrame::FieldsFrame()
  : wxFrame(nullptr, wxID_ANY, "Minesweeper", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER) // NOLINT(hicpp-signed-bitwise)
  , game{minesweeper::Minesweeper::create(gameLevel).value()} {

  auto menuFile = std::make_unique<wxMenu>();
  menuFile->Append(toInt(NewGameId::LastLevel), "&New game\tCtrl-N", "Start new game");
  menuFile->AppendSeparator();
  menuFile->AppendRadioItem(toInt(NewGameId::Beginner), "Beginner");
  menuFile->AppendRadioItem(toInt(NewGameId::Intermediate), "Intermediate");
  menuFile->AppendRadioItem(toInt(NewGameId::Expert), "Expert");
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
  Bind(wxEVT_MENU, &FieldsFrame::onNewGame, this, toInt(NewGameId::LastLevel));
  Bind(wxEVT_MENU, &FieldsFrame::onNewGame, this, toInt(NewGameId::Beginner));
  Bind(wxEVT_MENU, &FieldsFrame::onNewGame, this, toInt(NewGameId::Intermediate));
  Bind(wxEVT_MENU, &FieldsFrame::onNewGame, this, toInt(NewGameId::Expert));
  Bind(wxEVT_MENU, &FieldsFrame::onAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &FieldsFrame::onExit, this, wxID_EXIT);

  std::unique_ptr<wxBoxSizer> topSizer{new wxBoxSizer(wxVERTICAL)};
  auto fieldHolderPanelOwner = std::make_unique<wxPanel>(this);
  topSizer->Add(fieldHolderPanelOwner.get());
  fieldHolderPanel = fieldHolderPanelOwner.release();
  SetSizerAndFit(topSizer.get());
  static_cast<void>(topSizer.release());

  createFields();
}

FieldPanel &FieldsFrame::getFieldPanel(FieldPanels &panels, uint64_t row, uint64_t col,
                                       const minesweeper::Minesweeper::Dimension &size) {
  return panels[row * size.width + col];
}

FieldBitmaps FieldsFrame::getDefaultBitmaps() {
  static constexpr auto bitmapSize = 25;

  FieldBitmaps bitmaps{bitmapSize};
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
      // NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall)
      bitmaps.getBackground(binding.first) = wxBitmap(wxImage(streamToReadFrom));
    }
  }

  {
    const auto figureImageBindings = std::map<FigureType, std::string>{
        {FigureType::Empty, "empty.png"}, {FigureType::One, "one.png"},          {FigureType::Two, "two.png"},
        {FigureType::Three, "three.png"}, {FigureType::Four, "four.png"},        {FigureType::Five, "five.png"},
        {FigureType::Six, "six.png"},     {FigureType::Seven, "seven.png"},      {FigureType::Eight, "eight.png"},
        {FigureType::Mine, "mine.png"},   {FigureType::WrongMine, "mine_2.png"}, {FigureType::Flag, "flag.png"},
    };

    for (const auto &binding: figureImageBindings) {
      auto image = fs.open(binding.second);
      auto streamToReadFrom = wxMemoryInputStream(image.begin(), image.size());
      bitmaps.getFigure(binding.first) = wxBitmap(wxImage(streamToReadFrom));
    }
  }
  return bitmaps;
}

void FieldsFrame::createFields() {
  fieldHolderPanel->Freeze();

  if (auto *sizer = fieldHolderPanel->GetSizer(); nullptr != sizer) {
    sizer->Clear(true);
    fieldHolderPanel->SetSizer(nullptr, true);
    panels.clear();
  }

  const auto size = this->game.getSize().value();

  auto gridSizerOwner = std::make_unique<wxGridSizer>(safeCast<int>(size.height), safeCast<int>(size.width), 0, 0);

  const auto width = static_cast<uint64_t>(size.width);
  const auto height = static_cast<uint64_t>(size.height);

  const auto fieldCount = width * height;
  const auto bitmapSize = bitmaps.getSize();

  auto fieldSize = wxSize(bitmapSize, bitmapSize);
  panels.reserve(fieldCount);
  for (uint64_t fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {

    auto fieldPanel =
        std::make_unique<FieldPanel>(*fieldHolderPanel, fieldIndex / width, fieldIndex % width, bitmaps, panels, game);

    fieldPanel->SetSize(fieldSize);
    fieldPanel->SetMinSize(fieldSize);
    fieldPanel->SetMaxSize(fieldSize);
    fieldPanel->SetWindowStyle(wxBORDER_NONE | wxBU_EXACTFIT); // NOLINT(hicpp-signed-bitwise)
    auto panelRef = std::ref(*fieldPanel);
    panels.push_back((panelRef));
    gridSizerOwner->Add(fieldPanel.get(), wxALL, 1);
    static_cast<void>(fieldPanel.release());
  }

  fieldHolderPanel->SetSizer(gridSizerOwner.get());
  static_cast<void>(gridSizerOwner.release());
  fieldHolderPanel->Layout();
  fieldHolderPanel->Thaw();
  GetSizer()->Fit(this);
}

void FieldsFrame::onExit(wxCommandEvent & /*unused*/) {
  Close(true);
}

// Because it is a binded event handler, in has to be a member function
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void FieldsFrame::onAbout(wxCommandEvent & /*unused*/) {
  wxMessageBox("This is the ultimate Minesweeper game!", "About Minesweeper",
               wxOK | wxICON_INFORMATION); // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
}

void FieldsFrame::onNewGame(wxCommandEvent &event) {
  gameLevel = [this, &event] {
    switch (toNewGameId(event.GetId())) {
    case NewGameId::Beginner:
      return minesweeper::GameLevel::Beginner;
    case NewGameId::Intermediate:
      return minesweeper::GameLevel::Intermediate;
    case NewGameId::Expert:
      return minesweeper::GameLevel::Expert;
    case NewGameId::LastLevel:
      return this->gameLevel;
    };
    throw std::runtime_error(fmt::format("Invalid value of NewGameId {}", event.GetId()));
  }();
  this->game = minesweeper::Minesweeper::create(gameLevel).value();
  createFields();
  Refresh();
}
} // namespace minesweeper_gui
