project(kickstart_alarm)
enable_testing()

# Header-only lib
set(KICKSTART_ALARM_SOURCES ${CMAKE_SOURCE_DIR}/.clang-format)

set(KICKSTART_ALARM_HEADERS include/KickStartAlarm.hpp)

set(KICKSTART_ALARM_DEMO_SOURCES src/main.cpp)

add_library(
  kickstart_alarm
  INTERFACE
)

# Dummy project to show the header-only lib in VS
add_custom_target(
  kickstart_alarm_proj
  EXCLUDE_FROM_ALL
  SOURCES include/KickStartAlarm.hpp
)

set_target_properties(
  kickstart_alarm_proj
  PROPERTIES FOLDER
             "kickstart/2019/practice"
)

# Demo
add_executable(kickstart_alarm_demo ${KICKSTART_ALARM_DEMO_SOURCES})

target_include_directories(kickstart_alarm INTERFACE include)

target_link_libraries(
  kickstart_alarm_demo
  kickstart_alarm
)

set_target_properties(
  kickstart_alarm_demo
  PROPERTIES FOLDER
             "kickstart/2019/practice"
)
