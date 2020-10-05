include(ToolchainIdentification)

if(NOT TI_IS_CLANG_CL)
  set(MINESWEEPER_GUI_HEADERS include/Utils.hpp include/WxWidgetsWrapper.hpp)
  set(MINESWEEPER_GUI_SOURCES src/main.cpp)

  add_executable(minesweeper_gui ${MINESWEEPER_GUI_HEADERS} ${MINESWEEPER_GUI_SOURCES})
  target_include_directories(minesweeper_gui PRIVATE include)
  target_link_libraries(
    minesweeper_gui
    PRIVATE cppminesweeper
            CONAN_PKG::wxwidgets
            CONAN_PKG::fmt
            CONAN_PKG::span-lite
            project_options
            project_warnings
  )

  if(WIN32)
    target_link_options(minesweeper_gui PRIVATE "/SUBSYSTEM:WINDOWS")
  endif()

  set_target_properties(minesweeper_gui PROPERTIES FOLDER "minesweeper")
endif()
