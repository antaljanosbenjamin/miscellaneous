find_package(
  Catch2
  REQUIRED
  CONFIG
)

include(ParseAndAddCatchTests)
include(Catch)

add_executable(limited_correspondence_test LetterAssemberTest.cpp)

target_link_libraries(
  limited_correspondence_test
  limited_correspondence
)

set_target_properties(
  limited_correspondence_test
  PROPERTIES
    FOLDER
    "kattis"
)
ParseAndAddCatchTests(limited_correspondence_test)