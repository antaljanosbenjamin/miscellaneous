project(advice)
enable_testing()

# Library
set(COLOR_FINDER_HEADERS include/ColorFinder.hpp include/Colors.hpp include/ColorFinderConstexpr.h)

add_library(color_finder INTERFACE)
target_include_directories(color_finder INTERFACE include)
target_link_libraries(color_finder INTERFACE project_options)
set(CONSTEXPR_STEPS 10000000)

message("AAA ${CMAKE_CXX_COMPILER_ID}")
if(MSVC)
  target_compile_options(color_finder INTERFACE /constexpr:steps${CONSTEXPR_STEPS})
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  target_compile_options(color_finder INTERFACE -fconstexpr-steps=${CONSTEXPR_STEPS})
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  target_compile_options(color_finder INTERFACE -fconstexpr-loop-limit=${CONSTEXPR_STEPS})
endif()

# Dummy project to show the header-only lib in VS
add_custom_target(
  color_finder_
  EXCLUDE_FROM_ALL
  SOURCES ${COLOR_FINDER_HEADERS}
)

set_target_properties(color_finder_ PROPERTIES FOLDER "color_finder")

# Demo
set(COLOR_FINDER_DEMO_SOURCES src/main.cpp)

add_executable(color_finder_demo ${COLOR_FINDER_DEMO_SOURCES})

target_link_libraries(color_finder_demo PRIVATE color_finder project_options project_warnings)

set_target_properties(color_finder_demo PROPERTIES FOLDER "color_finder")
