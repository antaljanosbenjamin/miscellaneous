#pragma once

#include <unordered_set>

#include "Properties.hpp"
#include "Todo.hpp"
#include "TodoPredicate.hpp"

namespace EntityStore {

class IStore {
public:
  IStore() = default;
  // Defaulting the copy and move operations doesn't mean that all of derived classes must be copyable/movable etc. It
  // just means that they can be copyable/movable etc.
  IStore(const IStore &) = default;
  IStore(IStore &&) = default;
  IStore &operator=(const IStore &) = default;
  IStore &operator=(IStore &&) = default;
  virtual ~IStore() = default;

  virtual bool insert(const TodoId id, Properties &&properties) = 0;
  virtual bool insert(const TodoId id, const Properties &properties) = 0;

  virtual const Properties *update(const TodoId id, Properties &&properties) = 0;
  virtual const Properties *update(const TodoId id, const Properties &properties) = 0;

  [[nodiscard]] virtual bool contains(const TodoId id) const = 0;
  [[nodiscard]] virtual const Properties *tryGet(const TodoId id) const = 0;
  [[nodiscard]] virtual const Properties &get(const TodoId id) const = 0;

  virtual bool remove(const TodoId id) = 0;

  [[nodiscard]] virtual std::unordered_set<TodoId> filterIds(const TodoPredicate &predicate) const = 0;

  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual void shrink() = 0;
};

} // namespace EntityStore
