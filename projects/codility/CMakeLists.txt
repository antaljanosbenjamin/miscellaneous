add_executable(rhodium2019 rhodium2019.cpp)
target_link_libraries(rhodium2019 PRIVATE project_options project_warnings)
set_target_properties(rhodium2019 PROPERTIES FOLDER "codility")

add_executable(ruthenium2019 ruthenium2019.cpp)
target_link_libraries(ruthenium2019 PRIVATE project_options project_warnings)
set_target_properties(ruthenium2019 PROPERTIES FOLDER "codility")
