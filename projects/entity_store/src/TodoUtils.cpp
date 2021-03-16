#include "TodoUtils.hpp"

namespace EntityStore {

void printProperties(std::ostream &os, const Properties &properties, const std::string &prefix) {
  for (std::underlying_type_t<PropertyId> propertyIndex{0u}; propertyIndex <= asUnderlying(PropertyId::_LAST);
       ++propertyIndex) {
    auto propertyId = static_cast<PropertyId>(propertyIndex);
    if (properties.hasProperty(propertyId)) {
      os << prefix << propertyInfos[propertyIndex].name << " => ";
      properties.visit(propertyId, [&os](const auto &value) { os << value << '\n'; });
    }
  }
}

void printTodo(std::ostream &os, const Store &store, TodoId id) {
  os << "Todo{" << id << "}\n";
  const auto propertiesPtr = store.tryGet(id);
  if (propertiesPtr == nullptr) {
    os << "\tIsn't in store!\n";
  } else {
    printProperties(os, *store.tryGet(id), "\t");
  }
}

std::ostream &operator<<(std::ostream &os, const Properties &properties) {
  printProperties(os, properties);
  return os;
}

std::ostream &operator<<(std::ostream &os, const Todo &todo) {
  os << "Todo{" << todo.id() << "}\n";
  printProperties(os, todo.properties, "\t");
  return os;
}

} // namespace EntityStore
