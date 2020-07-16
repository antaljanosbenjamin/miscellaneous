function(add_dummy_project_for_header_only_lib DUMMY_TARGET_NAME LIB_HEADERS)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "LIB_FOLDER"
    ""
    ${ARGN}
  )
  set(LIB_FOLDER ${PARSED_ARGS_LIB_FOLDER})
  # Dummy project to show the header-only lib in VS
  add_custom_target(
    ${DUMMY_TARGET_NAME}
    EXCLUDE_FROM_ALL
    SOURCES ${LIB_HEADERS}
  )

  if(${LIB_FOLDER})
    set_target_properties(${DUMMY_TARGET_NAME} PROPERTIES FOLDER ${LIB_FOLDER})
    message(STATUS "FOLDER property of ${DUMMY_TARGET_NAME} is to ${LIB_FOLDER}")
  endif()

endfunction()
