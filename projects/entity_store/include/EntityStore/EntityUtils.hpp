#pragma once

#include <iostream>

#include "EntityStore/Internal/Entity.hpp"
#include "EntityStore/Properties.hpp"
#include "EntityStore/Property.hpp"
#include "EntityStore/Store.hpp"

namespace EntityStore {

void printProperties(std::ostream &os, const Properties &properties);
void printPropertiesWithPrefix(std::ostream &os, const Properties &properties, const std::string &prefix);

void printEntity(std::ostream &os, const Store &store, EntityId id);

std::ostream &operator<<(std::ostream &os, const Properties &properties);

std::ostream &operator<<(std::ostream &os, const Entity &entity);

} // namespace EntityStore
