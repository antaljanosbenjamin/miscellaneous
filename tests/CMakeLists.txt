if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_subdirectory(catch_main)

set(UNIT_TEST_PREFIX "unittests.")

add_subdirectory(minesweeper)
add_subdirectory(color_finder)
add_subdirectory(entity_store)
add_subdirectory(kattis)
add_subdirectory(kickstart)
