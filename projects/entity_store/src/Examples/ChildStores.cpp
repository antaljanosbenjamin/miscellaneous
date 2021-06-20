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
