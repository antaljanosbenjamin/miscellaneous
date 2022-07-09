add_executable(matrix_connected_components_tests AlgorithmTests.cpp)

target_link_libraries(
  matrix_connected_components_tests
  PRIVATE matrix_connected_components
          utils
          project_options
          project_warnings
          catch_main
)
set_target_properties(
  matrix_connected_components_tests PROPERTIES FOLDER "matrix_connected_components"
)

catch_discover_tests(
  matrix_connected_components_tests TEST_PREFIX
  "${UNIT_TEST_PREFIX}matrix_connected_components.algorithm."
)
