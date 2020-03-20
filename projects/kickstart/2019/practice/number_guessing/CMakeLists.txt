add_executable(number_guessing main.cpp)

target_link_libraries(number_guessing PRIVATE project_options project_warnings)

set_target_properties(number_guessing PROPERTIES FOLDER "kickstart/2019/practice")
