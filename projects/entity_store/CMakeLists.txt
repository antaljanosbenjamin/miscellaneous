add_library(
  entity_store
  include/EntityStore/EntityUtils.hpp
  include/EntityStore/Internal/Entity.hpp
  include/EntityStore/Internal/EntityPredicate.hpp
  include/EntityStore/Internal/EntityStatesManager.hpp
  include/EntityStore/Internal/IStore.hpp
  include/EntityStore/Internal/NestedStore.hpp
  include/EntityStore/Internal/RootStore.hpp
  include/EntityStore/Properties.hpp
  include/EntityStore/Property.hpp
  include/EntityStore/Store.hpp
  include/EntityStore/StoreExceptions.hpp
  src/EntityStore/EntityUtils.cpp
  src/EntityStore/Internal/Entity.cpp
  src/EntityStore/Internal/EntityStatesManager.cpp
  src/EntityStore/Internal/NestedStore.cpp
  src/EntityStore/Internal/RootStore.cpp
  src/EntityStore/Properties.cpp
  src/EntityStore/Property.cpp
  src/EntityStore/Store.cpp
  src/EntityStore/StoreExceptions.cpp
)

target_include_directories(entity_store PUBLIC include)
target_link_libraries(entity_store PUBLIC project_options utils)
target_link_libraries(entity_store PRIVATE project_warnings)
set_target_properties(entity_store PROPERTIES FOLDER "entity_store")

add_executable(entity_store_demo src/main.cpp)
target_link_libraries(entity_store_demo PUBLIC entity_store)
target_link_libraries(entity_store_demo PRIVATE project_options project_warnings)
set_target_properties(entity_store_demo PROPERTIES FOLDER "entity_store")

add_executable(entity_store_example_the_basic_store src/Examples/TheBasicStore.cpp)
target_link_libraries(entity_store_example_the_basic_store PUBLIC entity_store)
target_link_libraries(entity_store_example_the_basic_store PRIVATE project_options)
set_target_properties(entity_store_example_the_basic_store PROPERTIES FOLDER "entity_store")

add_executable(entity_store_example_queries src/Examples/Queries.cpp)
target_link_libraries(entity_store_example_queries PUBLIC entity_store)
target_link_libraries(entity_store_example_queries PRIVATE project_options)
set_target_properties(entity_store_example_queries PROPERTIES FOLDER "entity_store")

add_executable(entity_store_example_child_stores src/Examples/ChildStores.cpp)
target_link_libraries(entity_store_example_child_stores PUBLIC entity_store)
target_link_libraries(entity_store_example_child_stores PRIVATE project_options)
set_target_properties(entity_store_example_child_stores PROPERTIES FOLDER "entity_store")
