#include "StoreExceptions.hpp"

namespace EntityStore {

DoesNotHaveTodoException::DoesNotHaveTodoException(const TodoId id)
  : std::out_of_range("Store does not contain Todo with id " + std::to_string(id)) {
}

InvalidRangeException::InvalidRangeException()
  : std::logic_error("The specified range is invalid!") {
}

} // namespace EntityStore
