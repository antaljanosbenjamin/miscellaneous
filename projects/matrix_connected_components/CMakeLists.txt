# Library
set(MATRIX_CONNECTED_COMPONENTS_SOURCES src/Algorithm.cpp)
set(MATRIX_CONNECTED_COMPONENTS_HEADERS include/Algorithm.hpp include/MatrixSlice.hpp)

add_library(
  matrix_connected_components ${MATRIX_CONNECTED_COMPONENTS_HEADERS}
                              ${MATRIX_CONNECTED_COMPONENTS_SOURCES}
)
target_include_directories(matrix_connected_components PUBLIC include/)

target_link_libraries(matrix_connected_components PUBLIC project_options utils)
target_link_libraries(matrix_connected_components PRIVATE project_warnings)

set_target_properties(matrix_connected_components PROPERTIES FOLDER "matrix_connected_components")

# Demo
set(MATRIX_CONNECTED_COMPONENTS_DEMO_SOURCES src/main.cpp)
add_executable(matrix_connected_components_demo ${MATRIX_CONNECTED_COMPONENTS_DEMO_SOURCES})

target_link_libraries(
  matrix_connected_components_demo matrix_connected_components project_options project_warnings
)

set_target_properties(
  matrix_connected_components_demo PROPERTIES FOLDER "matrix_connected_components"
)
