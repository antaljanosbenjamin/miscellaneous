find_package(
  Catch2
  REQUIRED
  CONFIG
)

include(ParseAndAddCatchTests)
include(Catch)

add_executable(advice_test AdviceTest.cpp)

target_link_libraries(
  advice_test
  advice
)

set_target_properties(
  advice_test
  PROPERTIES FOLDER
             "kattis"
)
parseandaddcatchtests(advice_test)
