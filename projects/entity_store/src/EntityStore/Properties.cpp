#include "EntityStore/Properties.hpp"

#include <utility>

namespace EntityStore {

DoesNotHavePropertyException::DoesNotHavePropertyException(const std::string_view propertyName)
  : std::out_of_range(std::string("Properties does not contain ") + propertyName.data() + " property!") {
}

bool Properties::hasProperty(const PropertyId propertyId) const {
  return m_propertyMap.find(propertyId) != m_propertyMap.end();
}

void Properties::update(Properties &&properties) {
  properties.m_propertyMap.merge(std::move(m_propertyMap));
  m_propertyMap = std::move(properties.m_propertyMap);
}

void Properties::update(const Properties &properties) {
  for (const auto &[id, property]: properties.m_propertyMap) {
    m_propertyMap.insert_or_assign(id, property);
  }
}

bool operator==(const Properties &lhs, const Properties &rhs) {
  return lhs.m_propertyMap == rhs.m_propertyMap;
}

} // namespace EntityStore
