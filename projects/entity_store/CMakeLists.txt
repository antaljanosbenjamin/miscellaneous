add_library(
  entity_store
  include/IStore.hpp
  include/NestedStore.hpp
  include/Properties.hpp
  include/Property.hpp
  include/RootStore.hpp
  include/Store.hpp
  include/StoreExceptions.hpp
  include/Todo.hpp
  include/TodoPredicate.hpp
  include/TodoStatesManager.hpp
  include/TodoUtils.hpp
  src/NestedStore.cpp
  src/Properties.cpp
  src/Property.cpp
  src/RootStore.cpp
  src/Store.cpp
  src/StoreExceptions.cpp
  src/Todo.cpp
  src/TodoStatesManager.cpp
  src/TodoUtils.cpp
)

target_include_directories(entity_store PUBLIC include)

target_link_libraries(
  entity_store
  project_options
  project_warnings
)

add_executable(entity_store_demo src/main.cpp)

target_link_libraries(entity_store_demo PUBLIC entity_store)
target_link_libraries(entity_store_demo PRIVATE project_options project_warnings)
