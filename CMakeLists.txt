cmake_minimum_required(VERSION 3.1.3)
project(miscellaneous)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(Warnings)

include(StaticAnalyzers)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

include(Conan)
run_conan()

include(CTest)
enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")

add_subdirectory(rust_projects)
add_subdirectory(projects)
add_subdirectory(tests)
add_subdirectory(experiments)
