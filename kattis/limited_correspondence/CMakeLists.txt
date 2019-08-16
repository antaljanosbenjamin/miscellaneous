set (LIMITED_CORREPONDENCE_SOURCES
		${CMAKE_SOURCE_DIR}/.clang-format
		LetterAssembler.hpp
		main.cpp
		)
		
add_executable (limited_correpondence ${LIMITED_CORREPONDENCE_SOURCES})
set_target_properties(limited_correpondence PROPERTIES FOLDER "kattis")
