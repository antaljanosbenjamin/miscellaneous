add_executable(convex_hull convex_hull_area.cpp)
target_link_libraries(convex_hull PRIVATE project_options project_warnings)
set_target_properties(convex_hull PROPERTIES FOLDER "Others")

add_executable(reverse_bits reverse_bits.cpp)
target_link_libraries(reverse_bits PRIVATE project_options project_warnings)
set_target_properties(reverse_bits PROPERTIES FOLDER "Others")

add_executable(sea_coast sea_coast.cpp)
target_link_libraries(sea_coast PRIVATE project_options project_warnings)
set_target_properties(sea_coast PROPERTIES FOLDER "Others")
