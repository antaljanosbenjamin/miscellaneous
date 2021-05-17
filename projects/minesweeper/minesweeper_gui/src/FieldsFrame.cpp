#include "FieldsFrame.hpp"

#include <map>
#include <string>

#include "BaseState.hpp"
#include "FigureType.hpp"
#include "Resources.hpp"
#include "utils/SafeCast.hpp"

namespace {
enum {
  ID_NewGame = 1,
};

minesweeper_gui::FieldBitmaps getDefaultBitmaps() {
  static constexpr auto bitmapSize = 25;
  using FieldBitmaps = minesweeper_gui::FieldBitmaps;
  using BaseState = minesweeper_gui::BaseState;
  using FigureType = minesweeper_gui::FigureType;

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
} // namespace

namespace minesweeper_gui {
FieldsFrame::FieldsFrame()
  : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
  , bitmaps{getDefaultBitmaps()}
  , game{minesweeper::Minesweeper::create(minesweeper::GameLevel::Beginner).value()} { // NOLINT(hicpp-signed-bitwise)

  auto menuFile = std::make_unique<wxMenu>();
  menuFile->Append(ID_NewGame, "&New game\tCtrl-N", "Start new game");
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
  Bind(wxEVT_MENU, &FieldsFrame::OnNewGame, this, ID_NewGame);
  Bind(wxEVT_MENU, &FieldsFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &FieldsFrame::OnExit, this, wxID_EXIT);

  std::unique_ptr<wxBoxSizer> topSizer{new wxBoxSizer(wxVERTICAL)};
  auto fieldHolderPanelOwner = std::make_unique<wxPanel>(this);
  topSizer->Add(fieldHolderPanelOwner.get());
  fieldHolderPanel = fieldHolderPanelOwner.release();
  SetSizerAndFit(topSizer.get());
  static_cast<void>(topSizer.release());

  CreateFields();
}

FieldPanel &FieldsFrame::getFieldPanel(FieldPanels &panels, uint64_t row, uint64_t col,
                                       const minesweeper::Minesweeper::Dimension &size) {
  return panels[row * size.width + col];
}

void FieldsFrame::CreateFields() {
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

void FieldsFrame::OnExit(wxCommandEvent & /*unused*/) {
  Close(true);
}

void FieldsFrame::OnAbout(wxCommandEvent & /*unused*/) {
  wxMessageBox("This is the ultimate Minesweeper game!", "About Minesweeper",
               wxOK | wxICON_INFORMATION); // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
}

void FieldsFrame::OnNewGame(wxCommandEvent & /*unused*/) {
  this->game = minesweeper::Minesweeper::create(minesweeper::GameLevel::Beginner).value();
  CreateFields();
  Refresh();
}
} // namespace minesweeper_gui
