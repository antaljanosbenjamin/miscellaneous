# EntityStore

`EntityStore` is meant to handle `Entity`s that can store different types properties as typed key-value pairs. `Entity`s can be stored, retrieved, deleted, and queried. `EntityStore` is an in-memory store component that provides this functionality.

## The basic store

The `Store` component is able to handle entities identified by a unique `int64_t id` and represented as a map of properties. It provides both compile time and runtime checked interfaces. With the compile time checked accessors, the id of the property id must be specified as a template parameter to prevent access to properties with not matching types (getting a double as string). In the runtime checked interface the property id is a regular function parameter, so if the type doesn't match, then an exception is thrown. A small example:

```cpp
#include <iostream>

#include <EntityStore/Store.hpp>

using PropertyId = EntityStore::PropertyId;
using Properties = EntityStore::Properties;

int main() {
  auto store = EntityStore::Store::create();

  store.insert(
      2133, Properties()
                .set<PropertyId::Title>("Darth Maul's lightsaber")
                .setAs(PropertyId::Description, std::string("One of the best!"))
                .setAs(PropertyId::Timestamp, 115.668));

  store.insert(2133, Properties().set<PropertyId::Title>(
                         "The second insert will fail for the same id"));

  const auto &properties = store.get(2133);

  const auto &compileTimeTypeCheckedTitle =
      store.get(2133).get<PropertyId::Title>();

  const auto &runtimeTypeCheckedTitle =
      store.get(2133).getAs<std::string>(PropertyId::Title);

  try {
    const auto invalidGetResult =
        store.get(2133).getAs<double>(PropertyId::Title);
  } catch (EntityStore::InvalidPropertyTypeException &exception) {
    std::cerr << "Property called " << exception.what()
              << " cannot be get as double!\n";
  }

  const auto *maybe_updated_properties = store.update(
      666, Properties().set<PropertyId::Title>("This should fail!"));
  if (nullptr == maybe_updated_properties) {
    std::cerr << "Couldn't update properties for entity with id 666\n";
  }

  if (store.contains(2133)) {
    std::cout << "Entity with id 2133 can be found!\n";
  };

  store.remove(2133);
  return 0;
}
```

## Queries

To be more than just a fancy key-value store, `Store` supports querying the entities by equality or by a range. The query interface also has compile time and runtime type checked version similarly to simple getter and setter. :

```cpp
#include <iostream>

#include <EntityStore/EntityUtils.hpp>
#include <EntityStore/Store.hpp>

using PropertyId = EntityStore::PropertyId;
using Properties = EntityStore::Properties;

int main() {
  auto store = EntityStore::Store::create();

  store.insert(
      1, Properties().set<PropertyId::Title>("Darth Zannah's lightsaber"));
  store.insert(2, Properties()
                      .set<PropertyId::Title>("Darth Bane's lightsaber")
                      .set<PropertyId::Description>("The really first one"));
  store.insert(3, Properties()
                      .set<PropertyId::Title>("Darth Bane's lightsaber")
                      .set<PropertyId::Description>(
                          "The second one for the other hand"));
  store.insert(4, Properties()
                      .set<PropertyId::Title>("Count Dooku's lightsaber")
                      .set<PropertyId::Description>("It's a curved one!"));
  store.insert(5, Properties().set<PropertyId::Timestamp>(4));
  store.insert(6, Properties().set<PropertyId::Timestamp>(5));
  store.insert(7, Properties().set<PropertyId::Timestamp>(666));
  store.insert(8, Properties().set<PropertyId::Timestamp>(6.99));
  store.insert(9, Properties().set<PropertyId::Timestamp>(4));

  const auto compileTimeCheckedDathBanesLightsabers =
      store.query<PropertyId::Title>("Darth Bane's lightsaber");

  std::cout << "Lightsabers of Darth Bane:\n";
  for (auto entityId: compileTimeCheckedDathBanesLightsabers) {
    EntityStore::printEntity(std::cout, store, entityId);
    std::cout << '\n';
  }

  const auto runtimeCheckedZannahsLightsabers = store.queryAs<std::string>(
      PropertyId::Title, "Darth Zannah's lightsaber");

  const auto compileTimeCheckedTimestamps =
      store.rangeQuery<PropertyId::Timestamp>(4.0, 6);

  std::cout << "Matching entities are\n";
  for (auto entityId: compileTimeCheckedTimestamps) {
    EntityStore::printEntity(std::cout, store, entityId);
    std::cout << '\n';
  }

  const auto runtimeCheckedTimestamps =
      store.rangeQuery<PropertyId::Timestamp>(4.0, 6.0);
  return 0;
}
```


## Child stores

Sometimes it’s very useful to have nested transactions – changes can be committed or thrown away in bulk. `Store` supports transactions by child stores.

```cpp
#include <iostream>

#include <EntityStore/Store.hpp>

using PropertyId = EntityStore::PropertyId;
using Properties = EntityStore::Properties;

int main() {
  auto store = EntityStore::Store::create();
  std::string description2133 = "One of the best!";
  store.insert(2133, Properties()
                         .set<PropertyId::Title>("Darth Maul's lightsaber")
                         .setAs(PropertyId::Description, description2133));

  // 1. Create a child store
  auto child = store.createChild();

  // 2. Use it to insert, get, update, delete, and query entities
  auto originalEntity = child.get(2133);

  child.update(2133,
               Properties().set<PropertyId::Title>("Master Yoda's lightsaber"));

  auto updatedEntity = child.get(2133);

  auto stillOriginalEntity = store.get(2133);

  auto queryResult = child.query<PropertyId::Title>("Master Yoda's lightsaber");

  // 3. Remove the child store and commit its changes to the parent
  child.commit();

  auto updatedEntityFromParent = store.get(2133);
  return 0;
}
```

For more examples please check the [demo](src/main.cpp), and for the complete interface please have a look at [header file](include/Store.hpp).