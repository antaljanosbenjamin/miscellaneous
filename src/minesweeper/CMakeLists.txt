set(
  MINESWEEPER_SOURCES
  ${CMAKE_SOURCE_DIR}/.clang-format
  main.cpp
)
find_package(Qt5 COMPONENTS Widgets DBus REQUIRED)

add_executable(minesweeper ${MINESWEEPER_SOURCES})
target_link_libraries(minesweeper Qt5::Widgets Qt5::DBus)
