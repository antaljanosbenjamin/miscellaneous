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
    --verbose
    --suppress=missingIncludeSystem
    --template='{file}:{line},{severity},{id},{message}'
    --project=${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/projects/
  )
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  add_custom_target(
    check
    COMMAND ${CPPCHECK} ${CPPCHECK_ARGS}
    COMMENT "running cppcheck"
  )
endif()
