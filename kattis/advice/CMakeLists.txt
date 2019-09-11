set(
  ADVICE_SOURCES
  ${CMAKE_SOURCE_DIR}/.clang-format
  Advice.cpp
  Advice.hpp
  main.cpp
  Point.cpp
  Point.hpp
)

add_executable(advice ${ADVICE_SOURCES})
set_target_properties(advice PROPERTIES FOLDER "kattis")
