cmake_minimum_required(VERSION 3.0.2)

project(limited_correspondence)
enable_testing()

set(
  LIMITED_correspondence_SOURCES
  ${CMAKE_SOURCE_DIR}/.clang-format
  LetterAssembler.cpp
  LetterAssembler.hpp
)

set(LIMITED_correspondence_DEMO_SOURCES main.cpp)

add_library(limited_correspondence ${LIMITED_correspondence_SOURCES})
add_executable(
  limited_correspondence_demo
  ${LIMITED_correspondence_DEMO_SOURCES}
)
target_link_libraries(
  limited_correspondence_demo
  limited_correspondence
)

set_target_properties(
  limited_correspondence
  PROPERTIES
    FOLDER
    "kattis"
)

set_target_properties(
  limited_correspondence_demo
  PROPERTIES
    FOLDER
    "kattis"
)
