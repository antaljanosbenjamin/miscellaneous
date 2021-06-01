add_executable(cminesweeper_wrapper_test main.cpp)

target_link_libraries(
  cminesweeper_wrapper_test PRIVATE cminesweeper_wrapper project_options project_warnings
)

add_test(NAME "${UNIT_TEST_PREFIX}cminesweeper_wrapper_test"
         COMMAND $<TARGET_FILE:cminesweeper_wrapper_test>
)

set_target_properties(cminesweeper_wrapper_test PROPERTIES FOLDER "minesweeper")
