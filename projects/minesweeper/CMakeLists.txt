project(minesweeper)
enable_testing()
set(MINESWEEPER_SOURCES src/main.cpp)
set(MINESWEEPER_HEADERS include/Minesweeper.hpp)

add_executable(minesweeper ${MINESWEEPER_HEADERS} ${MINESWEEPER_SOURCES})
target_include_directories(minesweeper PUBLIC include)
target_link_libraries(minesweeper PUBLIC cminesweeper_shared)
set_target_properties(minesweeper PROPERTIES FOLDER "minesweeper")
