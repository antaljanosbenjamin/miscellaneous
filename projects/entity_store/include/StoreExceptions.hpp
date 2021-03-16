#pragma once

#include <stdexcept>

#include "Todo.hpp"

namespace EntityStore {

class DoesNotHaveTodoException : public std::out_of_range {
public:
  explicit DoesNotHaveTodoException(const TodoId id);
};

class InvalidRangeException : public std::logic_error {
public:
  explicit InvalidRangeException();
};

} // namespace EntityStore
