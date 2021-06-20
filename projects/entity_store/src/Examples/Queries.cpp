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
