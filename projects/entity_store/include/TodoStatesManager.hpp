#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Todo.hpp"

namespace EntityStore {

enum class TodoState {
  Default,
  OnlyUpdated,
  InsertedByThis,
  ReInsertedByThis,
  RemovedByThis,
};

class TodoStateHandler {
public:
  void insert();
  void update();
  void remove();

  [[nodiscard]] bool needsToInsertToParent() const;
  [[nodiscard]] bool needsToUpdateInParent() const;
  [[nodiscard]] bool needsToRemoveFromParent() const;
  [[nodiscard]] const TodoState &state() const;

private:
  TodoState m_state{TodoState::Default};
};

class TodoTransaction;

class TodoStatesManager {
public:
  friend TodoTransaction;
  using StateHandlerMap = std::unordered_map<TodoId, TodoStateHandler>;

  TodoStatesManager() = default;
  TodoStatesManager(const TodoStatesManager &) = default;
  TodoStatesManager(TodoStatesManager &&) = default;
  TodoStatesManager &operator=(const TodoStatesManager &) = default;
  TodoStatesManager &operator=(TodoStatesManager &&) = default;
  ~TodoStatesManager() = default;

  [[nodiscard]] const TodoStateHandler *tryGetState(const TodoId id) const;
  [[nodiscard]] const TodoStateHandler &getState(const TodoId id) const;

  [[nodiscard]] TodoTransaction startInsert(const TodoId id);
  [[nodiscard]] TodoTransaction startUpdate(const TodoId id);
  [[nodiscard]] TodoTransaction startRemove(const TodoId id);

  [[nodiscard]] std::unordered_set<TodoId> createTodoIdsSet() const;
  bool eraseStateHandler(const TodoId id);
  [[nodiscard]] const StateHandlerMap &stateHandlers() const;

private:
  StateHandlerMap m_stateHandlers;
};

class TodoTransaction {
public:
  friend TodoStatesManager;
  TodoTransaction() = delete;
  TodoTransaction(TodoTransaction &) = delete;
  TodoTransaction &operator=(TodoTransaction &) = delete;
  TodoTransaction &operator=(TodoTransaction &&) = delete;

  TodoTransaction(TodoTransaction &&) = default;

  ~TodoTransaction();
  void abort();

  TodoStateHandler &operator*();
  const TodoStateHandler &operator*() const;
  TodoStateHandler *operator->();
  const TodoStateHandler *operator->() const;

private:
  enum class Type {
    Insert,
    Update,
    Remove,
  };

  TodoTransaction(TodoStatesManager &manager, const TodoId id, Type type);

  TodoStatesManager &m_manager;
  const Type m_type;
  typename TodoStatesManager::StateHandlerMap::iterator m_handlerIt;
  bool m_aborted;
};

} // namespace EntityStore