option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

set(CLANG_TIDY_RUNNER "run-clang-tidy-10" CACHE STRING "Name of clang-tidy runner")

if(ENABLE_CLANG_TIDY)
  find_program(RUNNER ${CLANG_TIDY_RUNNER})
  if(RUNNER)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(clang-tidy ALL COMMAND ${RUNNER} COMMENT "running clang-tidy")
  else()
    message(SEND_ERROR "clang-tidy requested but clang-tidy runner is not found")
  endif()
endif()
