include(ToolchainIdentification)

if(NOT TI_IS_CLANG_CL)
  set(RESOURCE_LIB_NAME minesweeper_gui_resources)
  cmrc_add_resource_library(
    ${RESOURCE_LIB_NAME}
    WHENCE
    res
    res/boomed.png
    res/closed.png
    res/eight.png
    res/empty.png
    res/five.png
    res/flag.png
    res/four.png
    res/hovered.png
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
    ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_FOLDER_PREFIX}${RESOURCE_LIB_NAME}/.clang-tidy COPYONLY
  )

  set(MINESWEEPER_GUI_HEADERS
      # cmake-format: sortable
      include/BaseState.hpp
      include/FieldBitmaps.hpp
      include/FieldPanel.hpp
      include/FieldsFrame.hpp
      include/FieldState.hpp
      include/MouseStateHandler.hpp
      include/Resources.hpp
      include/utils/EnumHelper.hpp
      include/utils/SafeCast.hpp
      include/utils/Warnings.hpp
      include/WxWidgetsWrapper.hpp
  )

  set(MINESWEEPER_GUI_SOURCES
      # cmake-format: sortable
      src/FieldBitmaps.cpp
      src/FieldPanel.cpp
      src/FieldsFrame.cpp
      src/FieldState.cpp
      src/main.cpp
      src/MouseStateHandler.cpp
  )

  add_executable(minesweeper_gui ${MINESWEEPER_GUI_HEADERS} ${MINESWEEPER_GUI_SOURCES})
  target_include_directories(minesweeper_gui PRIVATE include)
  target_link_libraries(
    minesweeper_gui
    PRIVATE cppminesweeper
            minesweeper_gui_resources
            utils
            CONAN_PKG::wxwidgets
            CONAN_PKG::fmt
            project_options
            project_warnings
  )
  source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${MINESWEEPER_GUI_SOURCES}
  )
  source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${MINESWEEPER_GUI_HEADERS}
  )

  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/res PREFIX res)

  if(WIN32)
    target_link_options(minesweeper_gui PRIVATE "/SUBSYSTEM:WINDOWS")
  endif()

  set_target_properties(minesweeper_gui PROPERTIES FOLDER "minesweeper")
  set_target_properties(minesweeper_gui_resources PROPERTIES FOLDER "minesweeper")
endif()
