#pragma once

#include <unordered_map>
#include <unordered_set>

#include "EntityStore/Internal/Entity.hpp"

namespace EntityStore {

enum class EntityState {
  Default,
  OnlyUpdated,
  InsertedByThis,
  ReInsertedByThis,
  RemovedByThis,
};

class EntityStateHandler {
public:
  void insert();
  void update();
  void remove();

  [[nodiscard]] bool needsToInsertToParent() const;
  [[nodiscard]] bool needsToUpdateInParent() const;
  [[nodiscard]] bool needsToRemoveFromParent() const;
  [[nodiscard]] const EntityState &state() const;

private:
  EntityState m_state{EntityState::Default};
};

class EntityTransaction;

class EntityStatesManager {
public:
  friend EntityTransaction;
  using StateHandlerMap = std::unordered_map<EntityId, EntityStateHandler>;

  EntityStatesManager() = default;
  EntityStatesManager(const EntityStatesManager &) = default;
  EntityStatesManager(EntityStatesManager &&) = default;
  EntityStatesManager &operator=(const EntityStatesManager &) = default;
  // Cannot be noexcept because of std::unordered_map is not noexcept move constructible
  EntityStatesManager &operator=(EntityStatesManager &&) = default;
  ~EntityStatesManager() = default;

  [[nodiscard]] const EntityStateHandler *tryGetState(const EntityId id) const;
  [[nodiscard]] const EntityStateHandler &getState(const EntityId id) const;

  // TODO(antaljanosbenjamin) Add insert/update/remove tokens to make sure only the regarding operation can be done
  [[nodiscard]] EntityTransaction startInsert(const EntityId id);
  [[nodiscard]] EntityTransaction startUpdate(const EntityId id);
  [[nodiscard]] EntityTransaction startRemove(const EntityId id);

  [[nodiscard]] std::unordered_set<EntityId> createEntityIdsSet() const;
  bool eraseStateHandler(const EntityId id);
  [[nodiscard]] const StateHandlerMap &stateHandlers() const;

private:
  StateHandlerMap m_stateHandlers;
};

class EntityTransaction {
public:
  friend EntityStatesManager;
  EntityTransaction() = delete;
  EntityTransaction(EntityTransaction &) = delete;
  EntityTransaction(EntityTransaction &&) = delete;
  EntityTransaction &operator=(EntityTransaction &) = delete;
  EntityTransaction &operator=(EntityTransaction &&) = delete;

  ~EntityTransaction();
  // TODO(antaljanosbenjamin) Check if explicit commiting is feasible
  void abort();

  EntityStateHandler &operator*();
  const EntityStateHandler &operator*() const;
  EntityStateHandler *operator->();
  const EntityStateHandler *operator->() const;

private:
  enum class Type {
    Insert,
    Update,
    Remove,
  };

  EntityTransaction(EntityStatesManager &manager, const EntityId id, Type type);

  // TODO(antaljanosbenjamin) Use pointer here and remove const from m_type to make move work
  EntityStatesManager &m_manager;
  const Type m_type;
  typename EntityStatesManager::StateHandlerMap::iterator m_handlerIt;
  bool m_aborted;
};

} // namespace EntityStore