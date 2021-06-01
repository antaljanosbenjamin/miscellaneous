#include "Property.hpp"

namespace EntityStore {

InvalidPropertyTypeException::InvalidPropertyTypeException(const std::string &propertyName)
  : std::runtime_error("Property \"" + propertyName + "\" is not compatible with the requested type!") {
}

} // namespace EntityStore
