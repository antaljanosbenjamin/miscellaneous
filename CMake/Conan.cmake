include(ToolchainIdentification)

macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake  file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
         "${CMAKE_BINARY_DIR}/conan.cmake"
    )
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)
  set(CONAN_DEPENDENCIES catch2/2.11.0 tl-expected/20190710)
  set(CONAN_OPTIONS "")

  if(WIN32)
    list(APPEND CONAN_DEPENDENCIES propagate_const/1.0.0@public-conan/testing)
    conan_add_remote(NAME twonington URL https://api.bintray.com/conan/twonington/public-conan)
  endif()

  if(NOT TI_IS_CLANG_CL)
    list(
      APPEND
      CONAN_DEPENDENCIES
      wxwidgets/3.1.2@bincrafters/stable
      fmt/7.0.3
      span-lite/0.6.0
    )
    list(APPEND CONAN_OPTIONS wxwidgets:compatibility=3.1 wxwidgets:debugreport=False)
  endif()

  conan_cmake_run(
    REQUIRES
    ${CONAN_DEPENDENCIES}
    BASIC_SETUP
    CMAKE_TARGETS # individual targets to link to
    BUILD
    missing
    OPTIONS
    ${CONAN_OPTIONS}
  )
endmacro()
