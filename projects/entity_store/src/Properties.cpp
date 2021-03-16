#include "Properties.hpp"

#include <utility>

namespace EntityStore {

DoesNotHavePropertyException::DoesNotHavePropertyException(const std::string &propertyName)
  : std::out_of_range("Properties does not contain " + propertyName + " property!") {
}

bool Properties::hasProperty(const PropertyId propertyId) const {
  return m_propertyMap.find(propertyId) != m_propertyMap.end();
}

void Properties::update(Properties &&properties) {
  properties.m_propertyMap.merge(std::move(m_propertyMap));
  m_propertyMap = std::move(properties.m_propertyMap);
}

void Properties::update(const Properties &properties) {
  PropertyMap tmp{properties.m_propertyMap};
  tmp.merge(std::move(m_propertyMap));
  m_propertyMap = std::move(tmp);
}

bool operator==(const Properties &lhs, const Properties &rhs) {
  return lhs.m_propertyMap == rhs.m_propertyMap;
}

} // namespace EntityStore
