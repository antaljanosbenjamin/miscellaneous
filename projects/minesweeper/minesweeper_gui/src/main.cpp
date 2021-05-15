#include <memory>

#include "FieldsFrame.hpp"
#include "WxWidgetsWrapper.hpp"

namespace minesweeper_gui {
class MyApp : public wxApp {
public:
  bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp); // NOLINT(cert-err58-cpp, cppcoreguidelines-pro-type-static-cast-downcast)

std::unique_ptr<FieldsFrame> frame;

bool MyApp::OnInit() {
  wxImage::AddHandler(new wxPNGHandler{}); // NOLINT(cppcoreguidelines-owning-memory)
  frame = std::make_unique<FieldsFrame>();
  frame->Show(true);
  static_cast<void>(frame.release());
  return true;
}

} // namespace minesweeper_gui
