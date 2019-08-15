set (CONVEX_HULL_SOURCES
		${CMAKE_SOURCE_DIR}/.clang-format
		convex_hull_area.cpp
		)
		
add_executable (convex_hull ${CONVEX_HULL_SOURCES})
set_target_properties(convex_hull PROPERTIES FOLDER "Others")

set (REVERSE_BITS_SOURCES
		${CMAKE_SOURCE_DIR}/.clang-format
		reverse_bits.cpp
		)
		
add_executable (reverse_bits ${REVERSE_BITS_SOURCES})
set_target_properties(reverse_bits PROPERTIES FOLDER "Others")

set (SEA_COAST_SOURCES
		${CMAKE_SOURCE_DIR}/.clang-format
		sea_coast.cpp
		)
		
add_executable (sea_coast ${SEA_COAST_SOURCES})
set_target_properties(sea_coast PROPERTIES FOLDER "Others")
