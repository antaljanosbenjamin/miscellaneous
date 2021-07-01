add_library(utils include/utils/PropagateConst.hpp)

target_include_directories(utils PUBLIC include)
target_link_libraries(utils PUBLIC project_options)
if(TI_IS_CLANG_CL OR TI_IS_MSVC)
  target_link_libraries(utils PUBLIC propagate_const)
endif()
target_link_libraries(utils PRIVATE project_warnings)
set_target_properties(utils PROPERTIES FOLDER "utils")
