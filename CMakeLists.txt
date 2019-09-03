cmake_minimum_required (VERSION 3.0.2)

#Use solution folders. 
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

project (Snippets)
set (CMAKE_CXX_STANDARD 17)
if (MSVC)
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_subdirectory (komal)
add_subdirectory (kattis)
add_subdirectory (others)
add_subdirectory (task_systems)