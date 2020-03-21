set(S135_SOURCE s135.cpp)

add_executable(s135 ${S135_SOURCE})
target_link_libraries(s135 PRIVATE project_options project_warnings)
set_target_properties(s135 PROPERTIES FOLDER "KoMaL")

add_executable(s135_InputGenerator ${S135_SOURCE})
target_compile_definitions(s135_InputGenerator PRIVATE -DGENERATE_INPUT)
target_link_libraries(s135_InputGenerator PRIVATE project_options project_warnings)
set_target_properties(s135_InputGenerator PROPERTIES FOLDER "KoMaL")
