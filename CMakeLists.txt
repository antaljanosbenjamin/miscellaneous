cmake_minimum_required(VERSION 3.11.0)
project(miscellaneous)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

include(Warnings)
include(StaticAnalyzers)
include(ToolchainIdentification)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
if(TI_IS_MSVC)
  target_compile_definitions(project_options INTERFACE TI_IS_MSVC)
elseif(TI_IS_CLANG_CL)
  target_compile_definitions(project_options INTERFACE TI_IS_CLANG_CL)
elseif(TI_IS_GCC)
  target_compile_definitions(project_options INTERFACE TI_IS_GCC)
elseif(TI_IS_CLANG)
  target_compile_definitions(project_options INTERFACE TI_IS_CLANG)
endif()

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

include(External)
run_conan()

include(CTest)
enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CMakeRC)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")

add_subdirectory(rust_projects)
add_subdirectory(projects)
add_subdirectory(tests)
add_subdirectory(experiments)
