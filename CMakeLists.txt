cmake_minimum_required(VERSION 3.0.2)

project(miscellaneous)

# Use solution folders.
set_property(
  GLOBAL
  PROPERTY
    USE_FOLDERS
    ON
)

set(CMAKE_CXX_STANDARD 17)
if (MSVC)
  add_compile_options(
    /W4
    /WX
  )
else ()
  # lots of warnings and all warnings as errors
  add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Werror
  )
endif ()

include(CTest)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DMY_DEBUG)
endif ()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(projects)
add_subdirectory(tests)
add_subdirectory(experiments)
