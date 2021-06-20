add_executable(limited_correspondence_test LetterAssemberTest.cpp)

target_link_libraries(
  limited_correspondence_test PRIVATE catch_main project_options project_warnings
                                      limited_correspondence
)

catch_discover_tests(
  limited_correspondence_test TEST_PREFIX "${UNIT_TEST_PREFIX}limited_correspondence."
  # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)

set_target_properties(limited_correspondence_test PROPERTIES FOLDER "kattis")
