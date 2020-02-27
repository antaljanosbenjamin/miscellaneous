add_executable(
  task_systems
  main.cpp
  MultiQueuedTaskSystem.cpp
  NotificationQueue.cpp
  OracleTaskSystem.cpp
)

set_target_properties(
  task_systems
  PROPERTIES FOLDER
             "TaskSystems"
)
find_package(Threads)
target_link_libraries(
  task_systems
  ${CMAKE_THREAD_LIBS_INIT}
)
