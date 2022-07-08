set(UNIT_TEST_PREFIX "${UNIT_TEST_PREFIX}containers.")

function(add_utils_test TEST_NAME SRC_FILES)
  set(TARGET_NAME "${TEST_NAME}_test")
  add_executable(${TARGET_NAME} ${SRC_FILES})

  target_link_libraries(${TARGET_NAME} PRIVATE utils project_options project_warnings catch_main)
  set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "utils")

  catch_discover_tests(${TARGET_NAME} TEST_PREFIX "${UNIT_TEST_PREFIX}${TEST_NAME}.")

endfunction()

add_utils_test(matrix MatrixTests.cpp)
add_utils_test(disjoint_set DisjointSetTests.cpp)

# catch_discover_tests( disjoint_set_tests TEST_PREFIX "${UNIT_TEST_PREFIX}disjoint_set." #
# EXTRA_ARGS -s # --reporter=xml # --out=tests.xml ) catch_discover_tests( matrix_tests TEST_PREFIX
# "${UNIT_TEST_PREFIX}matrix." # EXTRA_ARGS -s # --reporter=xml # --out=tests.xml )
