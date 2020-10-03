# Library
set(ADVICE_SOURCES src/Advice.cpp src/Point.cpp)
set(ADVICE_HEADERS include/Advice.hpp include/Point.hpp)

add_library(advice ${ADVICE_HEADERS} ${ADVICE_SOURCES})
target_include_directories(advice PUBLIC include)
target_link_libraries(advice PUBLIC project_options)
target_link_libraries(advice PRIVATE project_warnings)

set_target_properties(advice PROPERTIES FOLDER "kattis")

# Demo
set(ADVICE_DEMO_SOURCES src/main.cpp)

add_executable(advice_demo ${ADVICE_DEMO_SOURCES})

target_link_libraries(advice_demo PRIVATE advice project_options project_warnings)

set_target_properties(advice_demo PROPERTIES FOLDER "kattis")
