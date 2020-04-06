option(
  ENABLE_CPPCHECK
  "Enable static analysis with cppcheck"
  OFF
)
option(
  ENABLE_CLANG_TIDY
  "Enable static analysis with clang-tidy"
  OFF
)
set(CLANG_TIDY_RUNNER
    "run-clang-tidy-9"
    CACHE STRING "Name of clang-tidy runner"
)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck HINTS /usr/local/bin)

  if(CPPCHECK)
    list(
      APPEND
      CPPCHECK_ARGS
      --enable=all
      --inconclusive
      --error-exitcode=1
      --language=c++
      --inline-suppr
      # cmake-format: off
    --template='{file}:{line} [{severity}/{id}]: {message}'
    # cmake-format: on
      --project=${CMAKE_BINARY_DIR}/compile_commands.json
    )
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(
      cppcheck ALL
      COMMAND ${CPPCHECK} ${CPPCHECK_ARGS}
      COMMENT "running cppcheck"
    )
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()

if(ENABLE_CLANG_TIDY)
  find_program(RUNNER ${CLANG_TIDY_RUNNER})
  if(RUNNER)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(
      clang-tidy ALL
      COMMAND ${RUNNER}
      COMMENT "running clang-tidy"
    )
  else()
    message(SEND_ERROR "clang-tidy requested but clang-tidy runner is not found")
  endif()
endif()
