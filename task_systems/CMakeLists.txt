add_executable (task_systems main.cpp
                OracleTaskSystem.h
                OracleTaskSystem.cpp
                MultiQueuedTaskSystem.h
                MultiQueuedTaskSystem.cpp
                NotificationQueue.h
                NotificationQueue.cpp
                TaskStealingTaskSystem.h
                TaskStealingTaskSystem.cpp)

set_target_properties(task_systems PROPERTIES FOLDER "TaskSystems")
find_package (Threads)
target_link_libraries (task_systems ${CMAKE_THREAD_LIBS_INIT})
