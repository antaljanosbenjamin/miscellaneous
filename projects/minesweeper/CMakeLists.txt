set(CPPMINESWEEPER_HEADERS include/Minesweeper.hpp)
set(CPPMINESWEEPER_SOURCES src/Minesweeper.cpp)

set(CPPMINESWEEPER_DEMO_SOURCES src/main.cpp)

add_library(cppminesweeper ${CPPMINESWEEPER_HEADERS} ${CPPMINESWEEPER_SOURCES})
target_include_directories(cppminesweeper PUBLIC include)
target_link_libraries(
  cppminesweeper
  PUBLIC cminesweeper_wrapper
         CONAN_PKG::propagate_const
         CONAN_PKG::tl-expected
         project_options
         project_warnings
)
target_link_libraries(cppminesweeper PRIVATE project_warnings)
set_target_properties(cppminesweeper PROPERTIES FOLDER "minesweeper")

add_executable(cppminesweeper_demo ${CPPMINESWEEPER_DEMO_SOURCES})
target_include_directories(cppminesweeper PUBLIC include)
target_link_libraries(cppminesweeper_demo PRIVATE cppminesweeper project_options project_warnings)
set_target_properties(cppminesweeper_demo PROPERTIES FOLDER "minesweeper")
