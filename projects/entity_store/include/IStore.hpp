#pragma once

#include <unordered_set>

#include "Entity.hpp"
#include "EntityPredicate.hpp"
#include "Properties.hpp"

namespace EntityStore {

// TODO(antaljanosbenjamin) Add proper documentation
class IStore { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual ~IStore() = default;

  virtual bool insert(const EntityId id, Properties &&properties) = 0;
  virtual bool insert(const EntityId id, const Properties &properties) = 0;

  virtual const Properties *update(const EntityId id, Properties &&properties) = 0;
  virtual const Properties *update(const EntityId id, const Properties &properties) = 0;

  [[nodiscard]] virtual bool contains(const EntityId id) const = 0;
  [[nodiscard]] virtual const Properties *tryGet(const EntityId id) const = 0;
  [[nodiscard]] virtual const Properties &get(const EntityId id) const = 0;

  virtual bool remove(const EntityId id) = 0;

  [[nodiscard]] virtual std::unordered_set<EntityId> filterIds(const EntityPredicate &predicate) const = 0;

  virtual void commit() = 0;
  virtual void rollback() = 0;
  virtual void shrink() = 0;
};

} // namespace EntityStore
