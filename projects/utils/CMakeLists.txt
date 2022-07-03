add_library(
  utils
  include/utils/Assert.hpp
  include/utils/Concepts.hpp
  include/utils/DisjointSets.hpp
  include/utils/Likely.hpp
  include/utils/Matrix.hpp
  include/utils/NotNull.hpp
  include/utils/PropagateConst.hpp
  src/Assert.cpp
)

target_include_directories(utils PUBLIC include)
target_link_libraries(utils PUBLIC project_options)
if(TI_IS_CLANG_CL OR TI_IS_MSVC)
  target_link_libraries(utils PUBLIC propagate_const)
endif()
target_link_libraries(utils PRIVATE project_warnings)
set_target_properties(utils PROPERTIES FOLDER "utils")
