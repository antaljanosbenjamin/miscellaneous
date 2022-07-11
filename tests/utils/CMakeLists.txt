set(UNIT_TEST_PREFIX "${UNIT_TEST_PREFIX}utils.")

add_executable(not_null_tests NotNullTests.cpp)

target_link_libraries(not_null_tests PRIVATE utils project_options project_warnings catch_main)
set_target_properties(not_null_tests PROPERTIES FOLDER "utils")

catch_discover_tests(
  not_null_tests TEST_PREFIX "${UNIT_TEST_PREFIX}not_null." # EXTRA_ARGS -s --reporter=xml
  # --out=tests.xml
)

add_subdirectory(containers)
