#include "EntityStore/Property.hpp"

namespace EntityStore {

InvalidPropertyTypeException::InvalidPropertyTypeException(std::string_view propertyName)
  : std::runtime_error(std::string("Property \"") + propertyName.data() +
                       "\" is not compatible with the requested type!") {
}

} // namespace EntityStore
