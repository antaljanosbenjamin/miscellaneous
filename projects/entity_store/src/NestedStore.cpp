#include "NestedStore.hpp"

#include "StoreExceptions.hpp"

namespace EntityStore {

NestedStore::NestedStore(IStore &parentStore)
  : m_parentStore{parentStore}
  , m_ownStore{}
  , m_statesManager{}
  , m_isCommitting{false} {
}
NestedStore::~NestedStore() {
  try {
    doCommitChanges();
  } catch (...) {
    // logging something is a better idea then doing nothing
  }
}

bool NestedStore::insert(const TodoId id, Properties &&properties) {
  return doInsert(id, std::move(properties));
}

bool NestedStore::insert(const TodoId id, const Properties &properties) {
  return doInsert(id, properties);
}

// The update function is totally correct without the second condition too, but it might be less efficient.
// The complexity of these algorithms can be compared by counting the lookups they require:
//
// in P means parent store contains a Todo with the specified id
// in O means own store contains a Todo with the specified id
// none means neither parent nor own store contain a Todo with the specified id
// P means lookup in parent store
// O means lookup in own store
// +---------------+---------------+---------------+
// | Case          |  w/ condition | w/o condition |
// +---------------+---------------+---------------+
// | in P          |          OPOO |           POO |
// +---------------+---------------+---------------+
// | in O          |            OO |            PO |
// +---------------+---------------+---------------+
// | in P, in O    |            OO |           POO |
// +---------------+---------------+---------------+
// | none          |            OP |            PO |
// +---------------+---------------+---------------+
// As the table shows, there is no obviously better solution. The better solution highly depends on the how the store is
// used. I assume that the parent store is significantly bigger, therefore the w/ version could perform better as it has
// to do a lookup in the parent store only when our own store does not contain the value. The code is slightly more
// complex, but it is not a big price.
const Properties *NestedStore::update(const TodoId id, Properties &&properties) {
  auto transaction = m_statesManager.startUpdate(id);
  if (transaction->state() == TodoState::RemovedByThis) {
    transaction.abort();
    return nullptr;
  }

  // The update would work without this condition too, but it might be more efficient in this way. Further explanation
  // please see the comment above
  if (m_ownStore.contains(id)) {
    return m_ownStore.update(id, std::move(properties));
  }

  auto parentPropertiesPtr = m_parentStore.tryGet(id);
  if (parentPropertiesPtr == nullptr) {
    transaction.abort();
    return nullptr;
  }

  m_ownStore.insert(id, *parentPropertiesPtr);
  return m_ownStore.update(id, std::move(properties));
}

// This function differs from the && version, see the comments:
const Properties *NestedStore::update(const TodoId id, const Properties &properties) {
  auto transaction = m_statesManager.startUpdate(id);
  if (transaction->state() == TodoState::RemovedByThis) {
    transaction.abort();
    return nullptr;
  }

  // The next few lines are different then the regarding lines in the && version of update. The reason for that is in
  // the && version we cannot move the properties twice, therefore, we cannot use the update function with the moved
  // properties without being 100% sure that the insert will succeed.
  auto updatedPropertiesPtr = m_ownStore.update(id, properties);
  if (updatedPropertiesPtr != nullptr) {
    return updatedPropertiesPtr;
  }

  auto parentPropertiesPtr = m_parentStore.tryGet(id);
  if (parentPropertiesPtr == nullptr) {
    transaction.abort();
    return nullptr;
  }

  m_ownStore.insert(id, *parentPropertiesPtr);
  return m_ownStore.update(id, properties);
}

bool NestedStore::contains(const TodoId id) const {
  return m_ownStore.contains(id) || (!isRemovedByThisChild(id) && m_parentStore.contains(id));
}

const Properties *NestedStore::tryGet(const TodoId id) const {
  auto propertiesPtr = m_ownStore.tryGet(id);
  if (propertiesPtr != nullptr) {
    return propertiesPtr;
  }

  if (isRemovedByThisChild(id)) {
    return nullptr;
  }
  return m_parentStore.tryGet(id);
}

const Properties &NestedStore::get(const TodoId id) const {
  auto propertiesPtr = tryGet(id);
  if (propertiesPtr == nullptr) {
    throw DoesNotHaveTodoException(id);
  }
  return *propertiesPtr;
}

bool NestedStore::remove(const TodoId id) {
  auto transaction = m_statesManager.startRemove(id);
  if (transaction->state() == TodoState::RemovedByThis) {
    transaction.abort();
    return false;
  }

  auto isRemoveSuccessfull = m_ownStore.remove(id);
  if (!isRemoveSuccessfull) {
    isRemoveSuccessfull = m_parentStore.contains(id);
  }

  if (!isRemoveSuccessfull) {
    transaction.abort();
  }

  return isRemoveSuccessfull;
}

std::unordered_set<TodoId> NestedStore::filterIds(const TodoPredicate &predicate) const {
  std::unordered_set<TodoId> result;
  result = m_ownStore.filterIds(predicate);
  result.merge(m_parentStore.filterIds(IgnoreIds{m_statesManager.createTodoIdsSet()} | predicate));
  return result;
}

void NestedStore::commit() {
  doCommitChanges();
  doRollback();
}

void NestedStore::rollback() {
  doRollback();
}

void NestedStore::shrink() {
  m_ownStore.shrink();
}

bool NestedStore::isRemovedByThisChild(const TodoId id) const {
  const auto *stateHandlerPtr = m_statesManager.tryGetState(id);
  return (stateHandlerPtr != nullptr && stateHandlerPtr->state() == TodoState::RemovedByThis);
}

void NestedStore::doCommitChanges() {
  if (m_isCommitting) {
    // The purpose of this is to prevent committing from destructor when the NestedStore is destructed because of an
    // exception is thrown from this function.
    return;
  }

  const auto removeWithCheck = [this](const TodoId id) {
    if (!m_parentStore.remove(id)) {
      throw std::logic_error("Cannot remove Todo while committing changes to parent!");
    }
  };

  const auto handleTodo = [this, &removeWithCheck](Todo &todo) {
    const auto &stateHandler = m_statesManager.getState(todo.id());

    if (stateHandler.needsToUpdateInParent()) {
      if (m_parentStore.update(todo.id(), std::move(todo.properties)) == nullptr) {
        throw std::logic_error("Cannot update Todo while committing changes to parent!");
      }
    } else {
      if (stateHandler.needsToRemoveFromParent()) {
        removeWithCheck(todo.id());
      }
      if (stateHandler.needsToInsertToParent()) {
        if (!m_parentStore.insert(todo.id(), std::move(todo.properties))) {
          throw std::logic_error("Cannot insert Todo while committing changes to parent!");
        }
      }
    }
  };

  m_isCommitting = true;
  for (auto &todoHolder: m_ownStore.m_todos) {
    if (todoHolder.has_value()) {
      auto &todo = *todoHolder;
      handleTodo(todo);
      m_statesManager.eraseStateHandler(todo.id());
    }
  }

  for (auto const &stateHandler: m_statesManager.stateHandlers()) {
    if (stateHandler.second.state() != TodoState::RemovedByThis) {
      throw std::logic_error("Todo is expected to be in RemovedByThis state, but it isn't!");
    }
    removeWithCheck(stateHandler.first);
  }
  m_isCommitting = false;
}

void NestedStore::doRollback() {
  m_ownStore = RootStore();
  m_statesManager = TodoStatesManager();
}

} // namespace EntityStore
