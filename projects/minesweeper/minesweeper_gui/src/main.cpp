#include <memory>

#include "FieldsFrame.hpp"
#include "WxWidgetsWrapper.hpp"

class MyApp : public wxApp {
public:
  bool OnInit() override;
  bool OnExceptionInMainLoop() override;
};

// NOLINTNEXTLINE(cert-err58-cpp, cppcoreguidelines-pro-type-static-cast-downcast)
wxIMPLEMENT_APP(MyApp); // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)

std::unique_ptr<minesweeper_gui::FieldsFrame> frame; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

bool MyApp::OnInit() {
  wxImage::AddHandler(new wxPNGHandler{}); // NOLINT(cppcoreguidelines-owning-memory)
  frame = std::make_unique<minesweeper_gui::FieldsFrame>();
  frame->Show(true);
  static_cast<void>(frame.release());
  return true;
}

bool MyApp::OnExceptionInMainLoop() {
  wxMessageBox("Something went wrong :(", "Error",
               wxOK | wxICON_ERROR); // NOLINT(hicpp-signed-bitwise, readability-magic-numbers)
  return true;
}