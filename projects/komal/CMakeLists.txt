set(S135_SOURCE
    ${CMAKE_SOURCE_DIR}/.clang-format
    s135.cpp
)

add_executable(s135 ${S135_SOURCE})
set_target_properties(
  s135
  PROPERTIES FOLDER
             "KoMaL"
)

add_executable(s135_InputGenerator ${S135_SOURCE})
set_target_properties(
  s135_InputGenerator
  PROPERTIES FOLDER
             "KoMaL"
)
target_compile_definitions(s135_InputGenerator PRIVATE -DGENERATE_INPUT)
