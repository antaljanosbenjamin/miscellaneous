#pragma once

#include <iostream>

#include "Entity.hpp"
#include "Properties.hpp"
#include "Property.hpp"
#include "Store.hpp"

namespace EntityStore {

// TODO(antaljanosbenjamin) Add printPropertiesWithPrefix
void printProperties(std::ostream &os, const Properties &properties, const std::string &prefix = "");

void printEntity(std::ostream &os, const Store &store, EntityId id);

std::ostream &operator<<(std::ostream &os, const Properties &properties);

std::ostream &operator<<(std::ostream &os, const Entity &entity);

} // namespace EntityStore
