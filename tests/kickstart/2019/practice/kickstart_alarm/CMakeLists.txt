find_package(
  Catch2
  REQUIRED
  CONFIG
)

include(ParseAndAddCatchTests)
include(Catch)

add_executable(kickstart_alarm_test KickStartAlarmTest.cpp)

target_link_libraries(
  kickstart_alarm_test
  kickstart_alarm
)

set_target_properties(
  kickstart_alarm_test
  PROPERTIES
    FOLDER
    "kickstart/2019/practice"
)
ParseAndAddCatchTests(kickstart_alarm_test)