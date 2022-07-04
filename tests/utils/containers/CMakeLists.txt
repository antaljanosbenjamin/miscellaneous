set(UNIT_TEST_PREFIX "${UNIT_TEST_PREFIX}containers.")
add_executable(disjoint_set_tests DisjointSetTests.cpp)

target_link_libraries(disjoint_set_tests PRIVATE utils project_options project_warnings catch_main)
set_target_properties(disjoint_set_tests PROPERTIES FOLDER "utils")

catch_discover_tests(
  disjoint_set_tests TEST_PREFIX "${UNIT_TEST_PREFIX}" # EXTRA_ARGS -s --reporter=xml
                                                       # --out=tests.xml
)
