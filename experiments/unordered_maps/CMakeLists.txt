set(UNORDERED_MAPS_SOURCES main.cpp memory_manager.cpp)
set(UNORDERED_MAPS_HEADERS main.cpp memory_manager.hpp)

option(ENABLE_MEMORY_TRACKING_FOR_UNORDERED_MAPS
       "Enable memory tracking of the unordered maps experiment, might mess up the timing" OFF
)

if(ENABLE_MEMORY_TRACKING_FOR_UNORDERED_MAPS)
  list(APPEND UNORDERED_MAPS_SOURCES memory_tracking.cpp)
endif()

add_executable(unordered_maps ${UNORDERED_MAPS_SOURCES} ${UNORDERED_MAPS_HEADERS})

set_target_properties(unordered_maps PROPERTIES FOLDER "unordered_maps")

target_link_libraries(
  unordered_maps PRIVATE project_options project_warnings CONAN_PKG::robin-hood-hashing
                         CONAN_PKG::benchmark
)

if(ENABLE_MEMORY_TRACKING_FOR_UNORDERED_MAPS)
  target_link_options(
    unordered_maps
    PRIVATE
    "-Wl,--wrap=malloc"
    "-Wl,--wrap=aligned_alloc"
    "-Wl,--wrap=realloc"
    "-Wl,--wrap=calloc"
    "-Wl,--wrap=free"
  )
endif()
