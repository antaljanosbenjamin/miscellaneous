project(minesweeper)
enable_testing()

# Library
set(MINESWEEPER_SOURCES src/main.cpp)
set(MINESWEEPER_HEADERS include/Minesweeper.hpp)

add_executable(minesweeper ${MINESWEEPER_HEADERS} ${MINESWEEPER_SOURCES})
target_include_directories(minesweeper PUBLIC include)
target_link_libraries(minesweeper PUBLIC libminesweeper.so)
target_link_directories(
  minesweeper
  PRIVATE
  /home/kovi/Repos/rust_projects/build/debug
)

set_target_properties(minesweeper PROPERTIES FOLDER "minesweeper")

# # Demo set(ADVICE_DEMO_SOURCES src/main.cpp)

# add_executable(advice_demo ${ADVICE_DEMO_SOURCES})

# target_link_libraries( advice_demo PRIVATE advice project_options project_warnings )

# set_target_properties(advice_demo PROPERTIES FOLDER "kattis")
