add_executable(
  task_systems
  main.cpp
  MultiQueuedTaskSystem.cpp
  MultiQueuedTaskSystem.hpp
  NotificationQueue.cpp
  NotificationQueue.hpp
  OracleTaskSystem.cpp
  OracleTaskSystem.hpp
)

set_target_properties(task_systems PROPERTIES FOLDER "TaskSystems")

find_package(Threads REQUIRED)

target_link_libraries(
  task_systems
  PRIVATE ${CMAKE_THREAD_LIBS_INIT}
          project_options
          project_warnings
)
