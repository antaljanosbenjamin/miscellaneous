function(set_project_warnings project_name)
  option(
    WARNINGS_AS_ERRORS
    "Treat compiler warnings as errors"
    TRUE
  )

  if(MSVC)
    target_compile_options(${project_name} INTERFACE /W4)
    if(WARNINGS_AS_ERRORS)
      if(${CMAKE_CXX_COMPILER_ID}
         STREQUAL
         "Clang"
         AND "x${CMAKE_CXX_SIMULATE_ID}"
             STREQUAL
             "xMSVC"
      )
        message(FATAL_ERROR "Cannot use warnings as errors with Clang on Windows!")
      endif()
      target_compile_options(${project_name} INTERFACE /WX)

    endif()
  else()
    target_compile_options(
      ${project_name}
      INTERFACE -Wall
                -Wextra
                -pedantic
                -Werror
    )
    if(WARNINGS_AS_ERRORS)
      target_compile_options(${project_name} INTERFACE -Werror)

      if(${CMAKE_CXX_COMPILER_ID}
         STREQUAL
         "Clang"
      )
        target_compile_options(${project_name} INTERFACE -Wno-unused-lambda-capture)
      endif()
    endif()
  endif()

endfunction()
