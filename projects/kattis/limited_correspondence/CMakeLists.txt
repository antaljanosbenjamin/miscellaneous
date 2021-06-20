# Library
set(LIMITED_CORRESPONDENCE_SOURCES src/LetterAssembler.cpp)
set(LIMITED_CORRESPONDENCE_HEADERS include/LetterAssembler.hpp)

add_library(
  limited_correspondence ${LIMITED_CORRESPONDENCE_HEADERS} ${LIMITED_CORRESPONDENCE_SOURCES}
)
target_include_directories(limited_correspondence PUBLIC include/)

target_link_libraries(limited_correspondence PUBLIC project_options)
target_link_libraries(limited_correspondence PRIVATE project_warnings)

set_target_properties(limited_correspondence PROPERTIES FOLDER "kattis")

# Demo
set(LIMITED_CORRESPONDENCE_DEMO_SOURCES src/main.cpp)
add_executable(limited_correspondence_demo ${LIMITED_CORRESPONDENCE_DEMO_SOURCES})

target_link_libraries(
  limited_correspondence_demo limited_correspondence project_options project_warnings
)

set_target_properties(limited_correspondence_demo PROPERTIES FOLDER "kattis")
