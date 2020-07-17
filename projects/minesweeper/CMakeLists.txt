project(minesweeper)

set(CPPMINESWEEPER_HEADERS include/Minesweeper.hpp)
set(CPPMINESWEEPER_SOURCES)
set(CPPMINESWEEPER_DEMO_SOURCES src/main.cpp)
set(CPPMINESWEEPER_DEMO_HEADERS)

add_library(cppminesweeper INTERFACE)
target_include_directories(cppminesweeper INTERFACE include)
target_link_libraries(cppminesweeper INTERFACE cminesweeper)
add_dummy_project_for_header_only_lib(
  cppminesweeper_ ${CPPMINESWEEPER_HEADERS} FOLDER "minesweeper"
)

add_executable(cppminesweeper_demo ${CPPMINESWEEPER_DEMO_HEADERS} ${CPPMINESWEEPER_DEMO_SOURCES})
target_link_libraries(cppminesweeper_demo PUBLIC cppminesweeper project_options project_warnings)
set_target_properties(cppminesweeper_demo PROPERTIES FOLDER "minesweeper")
