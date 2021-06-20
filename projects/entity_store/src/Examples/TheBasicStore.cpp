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
