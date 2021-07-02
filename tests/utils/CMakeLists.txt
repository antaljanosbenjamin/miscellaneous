add_executable(utils_test NotNullTests.cpp)

target_link_libraries(utils_test PRIVATE utils project_options project_warnings catch_main)

catch_discover_tests(
  utils_test TEST_PREFIX "${UNIT_TEST_PREFIX}" # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)
