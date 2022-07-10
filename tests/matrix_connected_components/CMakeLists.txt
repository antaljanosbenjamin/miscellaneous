set(UNIT_TEST_PREFIX "${UNIT_TEST_PREFIX}matrix_connected_components.")

add_library(matrix_connected_components_test_utils MatrixUtils.cpp MatrixUtils.hpp)
target_link_libraries(matrix_connected_components_test_utils PUBLIC project_options utils)
target_link_libraries(matrix_connected_components_test_utils PRIVATE project_warnings)
target_include_directories(matrix_connected_components_test_utils PUBLIC "${CMAKE_SOURCE_DIR}")

function(add_matrix_connected_component_test TEST_NAME SRC_FILES)
  set(TARGET_NAME "${TEST_NAME}_test")
  add_executable(${TARGET_NAME} ${SRC_FILES})

  target_link_libraries(
    ${TARGET_NAME}
    PRIVATE matrix_connected_components
            matrix_connected_components_test_utils
            project_options
            project_warnings
            catch_main
  )
  set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "matrix_connected_components")

  catch_discover_tests(${TARGET_NAME} TEST_PREFIX "${UNIT_TEST_PREFIX}${TEST_NAME}.")
endfunction()

add_matrix_connected_component_test(algorithm AlgorithmTests.cpp)
add_matrix_connected_component_test(matrix_slice MatrixSliceTests.cpp)
