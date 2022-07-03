add_library(
  utils
  include/utils/Assert.hpp
  include/utils/Concepts.hpp
  include/utils/containers/DisjointSets.hpp
  include/utils/containers/Matrix.hpp
  include/utils/Likely.hpp
  include/utils/NotNull.hpp
  include/utils/PropagateConst.hpp
  include/utils/task_system/NotificationQueue.hpp
  include/utils/task_system/TaskSystem.hpp
  src/Assert.cpp
  src/task_system/NotificationQueue.cpp
)

target_include_directories(utils PUBLIC include)
target_link_libraries(utils PUBLIC project_options)
if(TI_IS_CLANG_CL OR TI_IS_MSVC)
  target_link_libraries(utils PUBLIC propagate_const)
endif()
target_link_libraries(utils PRIVATE project_warnings)
set_target_properties(utils PROPERTIES FOLDER "utils")
