include(ToolchainIdentification)

macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake  file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake"
         "${CMAKE_BINARY_DIR}/conan.cmake"
    )
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  conan_add_remote(
    NAME bincrafters URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
  )
  set(CONAN_DEPENDENCIES catch2/2.13.8 tl-expected/20190710 robin-hood-hashing/3.11.5
                         benchmark/1.6.1
  )

  if(NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND CONAN_DEPENDENCIES jemalloc/5.2.1)
  endif()

  set(CONAN_OPTIONS "")

  if(NOT TI_IS_CLANG_CL)
    list(APPEND CONAN_DEPENDENCIES wxwidgets/3.1.5@bincrafters/stable fmt/8.1.1)
    list(
      APPEND
      CONAN_OPTIONS
      wxwidgets:compatibility=3.1
      wxwidgets:debugreport=False
      wxwidgets:webview=False
    )
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

if(WIN32)
  include(FetchContent)

  FetchContent_Populate(
    propagate_const GIT_REPOSITORY https://github.com/jbcoe/propagate_const.git
    GIT_TAG 672cdbcd27028f8985dc7a559a085e530ac4656b
  )

  add_subdirectory(${propagate_const_SOURCE_DIR} ${propagate_const_BINARY_DIR})
endif()
