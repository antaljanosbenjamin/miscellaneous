#include <memory>

#include "FieldsFrame.hpp"
#include "WxWidgetsWrapper.hpp"

class MyApp : public wxApp {
public:
  bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp); // NOLINT(cert-err58-cpp, cppcoreguidelines-pro-type-static-cast-downcast)

std::unique_ptr<minesweeper_gui::FieldsFrame> frame;

bool MyApp::OnInit() {
  wxImage::AddHandler(new wxPNGHandler{}); // NOLINT(cppcoreguidelines-owning-memory)
  frame = std::make_unique<minesweeper_gui::FieldsFrame>();
  frame->Show(true);
  static_cast<void>(frame.release());
  return true;
}
