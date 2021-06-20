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

target_link_libraries(entity_store project_options project_warnings)

add_executable(entity_store_demo src/main.cpp)

target_link_libraries(entity_store_demo PUBLIC entity_store)
target_link_libraries(entity_store_demo PRIVATE project_options project_warnings)
