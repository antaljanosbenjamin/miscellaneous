include(ToolchainIdentification)

function(set_project_warnings project_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)

  if(WIN32)
    target_compile_options(${project_name} INTERFACE /W4)
    if(WARNINGS_AS_ERRORS)
      if(TI_IS_CLANG_CL)
        message(FATAL_ERROR "Cannot use warnings as errors with Clang on Windows!")
      endif()

      target_compile_options(${project_name} INTERFACE /WX)
    endif()
  elseif(UNIX)
    target_compile_options(${project_name} INTERFACE -Wall -Wextra -pedantic -Werror)

    if(TI_IS_CLANG)
      target_compile_options(${project_name} INTERFACE -Wno-unused-lambda-capture)
    endif()

    if(WARNINGS_AS_ERRORS)
      target_compile_options(${project_name} INTERFACE -Werror)
    endif()
  endif()
endfunction()
