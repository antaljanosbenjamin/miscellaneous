add_executable(
  unordered_maps
  main.cpp
)

set_target_properties(unordered_maps PROPERTIES FOLDER "unordered_maps")

target_link_libraries(
  unordered_maps PRIVATE project_options project_warnings CONAN_PKG::robin-hood-hashing CONAN_PKG::benchmark
)
