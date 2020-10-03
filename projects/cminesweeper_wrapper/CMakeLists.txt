include(HeadorOnlyVSProject)

set(CMINESWEEPER_WRAPPER_HEADERS include/CMinesweeper.hpp)

add_library(cminesweeper_wrapper INTERFACE)
target_include_directories(cminesweeper_wrapper INTERFACE include)
target_link_libraries(cminesweeper_wrapper INTERFACE cminesweeper project_options)
add_dummy_project_for_header_only_lib(
  cminesweeper_wrapper_ ${CMINESWEEPER_WRAPPER_HEADERS} FOLDER "minesweeper"
)
