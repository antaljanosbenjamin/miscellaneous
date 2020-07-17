function(add_dummy_project_for_header_only_lib DUMMY_TARGET_NAME LIB_HEADERS)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "FOLDER"
    ""
    ${ARGN}
  )
  # Dummy project to show the header-only lib in VS
  add_custom_target(${DUMMY_TARGET_NAME} SOURCES ${LIB_HEADERS})

  if(PARSED_ARGS_FOLDER)
    message(STATUS "FOLDER property of ${DUMMY_TARGET_NAME} is to ${PARSED_ARGS_FOLDER}")
    set_target_properties(${DUMMY_TARGET_NAME} PROPERTIES FOLDER ${PARSED_ARGS_FOLDER})
  endif()

endfunction()
