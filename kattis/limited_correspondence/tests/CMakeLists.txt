find_package(
  Catch2
  REQUIRED
  CONFIG
)

include(ParseAndAddCatchTests)
include(Catch)

add_executable(limited_correspondence_test LetterAssemberTest.cpp)
target_include_directories(
  limited_correspondence_test
  PRIVATE ${limited_correspondence_SOURCE_DIR}
)
target_link_libraries(
  limited_correspondence_test
  limited_correspondence
)
