project(kickstart_alarm)
enable_testing()

set(KICKSTART_ALARM_DEMO_SOURCES src/main.cpp)

add_library(kickstart_alarm INTERFACE)
add_executable(
  kickstart_alarm_demo
  ${KICKSTART_ALARM_DEMO_SOURCES}
)

target_include_directories(
  kickstart_alarm
  INTERFACE
  include
)

target_link_libraries(
  kickstart_alarm_demo
  kickstart_alarm
)

source_group("kickstart\\2019\\practice" REGULAR_EXPRESSION include/KickStartAlarm.hpp)

set_target_properties(
  kickstart_alarm_demo
  PROPERTIES
    FOLDER
    "kickstart/2019/practice"
)
