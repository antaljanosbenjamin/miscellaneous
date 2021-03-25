include(ToolchainIdentification)

if(NOT TI_IS_CLANG_CL)
  cmrc_add_resource_library(
    minesweeper_gui_resources
    WHENCE
    res
    res/boomed.png
    res/closed.png
    res/eight.png
    res/empty.png
    res/five.png
    res/flag.png
    res/four.png
    res/hoovered.png
    res/mine.png
    res/mine_2.png
    res/one.png
    res/opened.png
    res/seven.png
    res/six.png
    res/three.png
    res/two.png
  )
  set(RESOURCE_FOLDER_PREFIX "__cmrc_")
  configure_file(
    res/.clang-tidy
    ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_FOLDER_PREFIX}minesweeper_gui_resources/.clang-tidy
    COPYONLY
  )

  set(MINESWEEPER_GUI_HEADERS include/Utils.hpp include/WxWidgetsWrapper.hpp)
  set(MINESWEEPER_GUI_SOURCES src/main.cpp)

  add_executable(minesweeper_gui ${MINESWEEPER_GUI_HEADERS} ${MINESWEEPER_GUI_SOURCES})
  target_include_directories(minesweeper_gui PRIVATE include)
  target_link_libraries(
    minesweeper_gui
    PRIVATE cppminesweeper
            minesweeper_gui_resources
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
  set_target_properties(minesweeper_gui_resources PROPERTIES FOLDER "minesweeper")
endif()
