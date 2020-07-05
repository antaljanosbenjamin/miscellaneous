cmake_minimum_required(VERSION 3.1.3)

project(miscellaneous)

include(CMake/Warnings.cmake)

include(CMake/StaticAnalyzers.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

include(CMake/Conan.cmake)
run_conan()

include(CTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")

add_subdirectory(rust_projects)
add_subdirectory(projects)
add_subdirectory(tests)
add_subdirectory(experiments)
