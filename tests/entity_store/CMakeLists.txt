add_executable(entity_store_test EntityStoreTest.cpp)

target_link_libraries(
  entity_store_test PRIVATE entity_store project_options project_warnings catch_main
)
set_target_properties(entity_store_test PROPERTIES FOLDER "entity_store")

catch_discover_tests(
  entity_store_test TEST_PREFIX "${UNIT_TEST_PREFIX}entity_store."
  # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)
