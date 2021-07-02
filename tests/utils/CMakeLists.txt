add_executable(utils_test NotNullTests.cpp)

target_link_libraries(utils_test PRIVATE utils project_options project_warnings catch_main)
set_target_properties(utils_test PROPERTIES FOLDER "utils")

catch_discover_tests(
  utils_test TEST_PREFIX "${UNIT_TEST_PREFIX}" # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)
