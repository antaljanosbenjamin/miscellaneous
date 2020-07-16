project(kickstart_alarm)

# Header-only lib
set(KICKSTART_ALARM_SOURCES)

set(KICKSTART_ALARM_HEADERS include/KickStartAlarm.hpp)

set(KICKSTART_ALARM_DEMO_SOURCES src/main.cpp)

add_library(kickstart_alarm INTERFACE)

add_dummy_project_for_header_only_lib(
  kickstart_alarm_ ${KICKSTART_ALARM_HEADERS} "kickstart/2019/practice"
)

# Demo
add_executable(kickstart_alarm_demo ${KICKSTART_ALARM_DEMO_SOURCES})

target_include_directories(kickstart_alarm INTERFACE include)

# Header-only library, so the project_warnings doesn't make sense, because it shouldn't be
# propagated to dependant targets
target_link_libraries(kickstart_alarm INTERFACE project_options)

target_link_libraries(kickstart_alarm_demo PRIVATE kickstart_alarm project_warnings project_options)

set_target_properties(kickstart_alarm_demo PROPERTIES FOLDER "kickstart/2019/practice")
