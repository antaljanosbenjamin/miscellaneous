
project(advice)
enable_testing()

set(
  ADVICE_SOURCES
  ${CMAKE_SOURCE_DIR}/.clang-format
  src/Advice.cpp
  src/Point.cpp
)

set(ADVICE_DEMO_SOURCES src/main.cpp)

add_library(advice ${ADVICE_SOURCES})
add_executable(
  advice_demo
  ${ADVICE_DEMO_SOURCES}
)

target_include_directories(
  advice
  PUBLIC
  include
)

target_link_libraries(
  advice_demo
  advice
)

set_target_properties(
  advice
  PROPERTIES
    FOLDER
    "kattis"
)

set_target_properties(
  advice_demo
  PROPERTIES
    FOLDER
    "kattis"
)
