#pragma once

#include <iostream>

#include "Properties.hpp"
#include "Property.hpp"
#include "Store.hpp"
#include "Todo.hpp"

namespace EntityStore {

void printProperties(std::ostream &os, const Properties &properties, const std::string &prefix = "");

void printTodo(std::ostream &os, const Store &store, TodoId id);

std::ostream &operator<<(std::ostream &os, const Properties &properties);

std::ostream &operator<<(std::ostream &os, const Todo &todo);

} // namespace EntityStore
