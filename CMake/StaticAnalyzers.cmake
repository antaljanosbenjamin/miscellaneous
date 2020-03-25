option(
  ENABLE_CPPCHECK
  "Enable static analysis with cppcheck"
  OFF
)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  list(
    APPEND
    CPPCHECK_ARGS
    --enable=all
    --inconclusive
    --error-exitcode=1
    --language=c++
    --inline-suppr
    --suppress=missingIncludeSystem
    # cmake-format: off
    --template='{file}:{line} [{severity}/{id}]: {message}'
    # cmake-format: on
    --project=${CMAKE_BINARY_DIR}/compile_commands.json
  )
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  add_custom_target(
    check ALL
    COMMAND ${CPPCHECK} ${CPPCHECK_ARGS}
    COMMENT "running cppcheck"
  )
endif()
