set(
  MINESWEEPER_SOURCES
  ${CMAKE_SOURCE_DIR}/.clang-format
  main.cpp
)

add_executable(minesweeper ${MINESWEEPER_SOURCES})
