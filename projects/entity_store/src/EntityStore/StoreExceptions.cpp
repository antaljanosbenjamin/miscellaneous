#include "EntityStore/StoreExceptions.hpp"

namespace EntityStore {

DoesNotHaveEntityException::DoesNotHaveEntityException(const EntityId id)
  : std::out_of_range("Store does not contain Entity with id " + std::to_string(id)) {
}

InvalidRangeException::InvalidRangeException()
  : std::logic_error("The specified range is invalid!") {
}

} // namespace EntityStore
