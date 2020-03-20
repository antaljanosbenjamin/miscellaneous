add_executable(mural main.cpp)

target_link_libraries(mural PRIVATE project_options project_warnings)

set_target_properties(mural PROPERTIES FOLDER "kickstart/2019/practice")
