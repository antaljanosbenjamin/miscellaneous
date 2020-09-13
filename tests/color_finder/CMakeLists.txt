add_executable(color_finder_test ColorFinderTest.cpp)

target_link_libraries(
  color_finder_test
  PRIVATE color_finder
          catch_main
          project_options
          project_warnings
)

catch_discover_tests(
  color_finder_test
  TEST_PREFIX
  "unittests.color_finder."
  # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)

set_target_properties(color_finder_test PROPERTIES FOLDER "color_finder")
