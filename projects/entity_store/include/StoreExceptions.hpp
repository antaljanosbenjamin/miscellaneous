#pragma once

#include <stdexcept>

#include "Entity.hpp"

namespace EntityStore {

class DoesNotHaveEntityException : public std::out_of_range {
public:
  explicit DoesNotHaveEntityException(const EntityId id);
};

class InvalidRangeException : public std::logic_error {
public:
  explicit InvalidRangeException();
};

} // namespace EntityStore
