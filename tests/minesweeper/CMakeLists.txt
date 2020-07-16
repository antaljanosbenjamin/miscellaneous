add_executable(cppminesweeper_test main.cpp)

target_link_libraries(cppminesweeper_test PRIVATE cppminesweeper project_options project_warnings)

add_test(NAME ${UNIT_TEST_PREFIX}cppminesweeper_test COMMAND $<TARGET_FILE:cppminesweeper_test>)

set_target_properties(cppminesweeper_test PROPERTIES FOLDER "minesweeper")
