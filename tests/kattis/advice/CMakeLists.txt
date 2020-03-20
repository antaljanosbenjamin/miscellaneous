add_executable(advice_test AdviceTest.cpp)

target_link_libraries(
  advice_test
  PRIVATE advice
          catch_main
          project_options
          project_warnings
)

catch_discover_tests(
  advice_test
  TEST_PREFIX
  "unittests."
  # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)

set_target_properties(advice_test PROPERTIES FOLDER "kattis")
