add_executable(kickstart_alarm_test KickStartAlarmTest.cpp)

target_link_libraries(
  kickstart_alarm_test PRIVATE kickstart_alarm catch_main project_options project_warnings
)

catch_discover_tests(
  kickstart_alarm_test TEST_PREFIX ${UNIT_TEST_PREFIX}
  # EXTRA_ARGS -s --reporter=xml --out=tests.xml
)

set_target_properties(kickstart_alarm_test PROPERTIES FOLDER "kickstart/2019/practice")
