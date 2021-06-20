#include "EntityStore/Internal/NestedStore.hpp"

#include "EntityStore/StoreExceptions.hpp"

namespace EntityStore {

template <SameAsProperties TProperties>
bool doInsert(const IStore &parentStore, RootStore &ownStore, EntityStatesManager &statesManager, const EntityId id,
              TProperties &&properties) {
  auto transaction = statesManager.startInsert(id);

  if (transaction->state() == EntityState::RemovedByThis) {
    ownStore.insert(id, std::forward<TProperties>(properties));
    return true;
  }
  if (parentStore.contains(id)) {
    transaction.abort();
    return false;
  }
  auto isOwnInsertSuccessfull = ownStore.insert(id, std::forward<TProperties>(properties));
  if (!isOwnInsertSuccessfull) {
    transaction.abort();
  }
  return isOwnInsertSuccessfull;
}

NestedStore::NestedStore(IStore &parentStore)
  : m_parentStore{parentStore}
  , m_ownStore{}
  , m_statesManager{}
  , m_isCommitting{false} {
}
NestedStore::~NestedStore() {
  try {
    // TODO(antaljanosbenjamin) Do not do anything in the destructor
    doCommitChanges();
  } catch (...) {
    // logging something is a better idea then doing nothing
  }
}

bool NestedStore::insert(const EntityId id, Properties &&properties) {
  return doInsert(m_parentStore, m_ownStore, m_statesManager, id, std::move(properties));
}

bool NestedStore::insert(const EntityId id, const Properties &properties) {
  return doInsert(m_parentStore, m_ownStore, m_statesManager, id, properties);
}

// The update function is totally correct without the second condition too, but it might be less efficient.
// The complexity of these algorithms can be compared by counting the lookups they require:
//
// in P means parent store contains an Entity with the specified id
// in O means own store contains an Entity with the specified id
// none means neither parent nor own store contains an Entity with the specified id
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
// As the table shows, there is no obviously better solution. The better solution highly depends on how the store is
// used. I assume that the parent store is significantly bigger, therefore the w/ version could perform better as it has
// to do a lookup in the parent store only when our own store does not contain the value. The code is slightly more
// complex, but it is not a big price.
const Properties *NestedStore::update(const EntityId id, Properties &&properties) {
  auto transaction = m_statesManager.startUpdate(id);
  if (transaction->state() == EntityState::RemovedByThis) {
    transaction.abort();
    return nullptr;
  }

  // The update would work without this condition too, but it might be more efficient in this way. For further
  // explanation please see the comment above
  if (m_ownStore.contains(id)) {
    return m_ownStore.update(id, std::move(properties));
  }

  const auto *parentPropertiesPtr = m_parentStore.tryGet(id);
  if (parentPropertiesPtr == nullptr) {
    transaction.abort();
    return nullptr;
  }

  m_ownStore.insert(id, *parentPropertiesPtr);
  return m_ownStore.update(id, std::move(properties));
}

// This function differs from the && version, see the comments:
const Properties *NestedStore::update(const EntityId id, const Properties &properties) {
  auto transaction = m_statesManager.startUpdate(id);
  if (transaction->state() == EntityState::RemovedByThis) {
    transaction.abort();
    return nullptr;
  }

  // TODO(antaljanosbenjamin) Make sure RootStore::update only moves when the update can happen and unify the two
  // version of NestedStore::update.

  // The next few lines are different then the regarding lines in the && version of update. The reason for that is in
  // the && version we cannot move the properties twice, therefore, we cannot use the update function with the moved
  // properties without being 100% sure that the insert will succeed.
  const auto *updatedPropertiesPtr = m_ownStore.update(id, properties);
  if (updatedPropertiesPtr != nullptr) {
    return updatedPropertiesPtr;
  }

  const auto *parentPropertiesPtr = m_parentStore.tryGet(id);
  if (parentPropertiesPtr == nullptr) {
    transaction.abort();
    return nullptr;
  }

  m_ownStore.insert(id, *parentPropertiesPtr);
  return m_ownStore.update(id, properties);
}

bool NestedStore::contains(const EntityId id) const {
  return m_ownStore.contains(id) || (!isRemovedByThisChild(id) && m_parentStore.contains(id));
}

const Properties *NestedStore::tryGet(const EntityId id) const {
  const auto *propertiesPtr = m_ownStore.tryGet(id);
  if (propertiesPtr != nullptr) {
    return propertiesPtr;
  }

  if (isRemovedByThisChild(id)) {
    return nullptr;
  }
  return m_parentStore.tryGet(id);
}

const Properties &NestedStore::get(const EntityId id) const {
  const auto *propertiesPtr = tryGet(id);
  if (propertiesPtr == nullptr) {
    throw DoesNotHaveEntityException(id);
  }
  return *propertiesPtr;
}

bool NestedStore::remove(const EntityId id) {
  auto transaction = m_statesManager.startRemove(id);
  if (transaction->state() == EntityState::RemovedByThis) {
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

std::unordered_set<EntityId> NestedStore::filterIds(const EntityPredicate &predicate) const {
  std::unordered_set<EntityId> result;
  result = m_ownStore.filterIds(predicate);
  result.merge(m_parentStore.filterIds(IgnoreIds{m_statesManager.createEntityIdsSet()} | predicate));
  return result;
}

void NestedStore::commit() {
  doCommitChanges();
  reset();
}

void NestedStore::rollback() {
  reset();
}

void NestedStore::shrink() {
  m_ownStore.shrink();
}

bool NestedStore::isRemovedByThisChild(const EntityId id) const {
  const auto *stateHandlerPtr = m_statesManager.tryGetState(id);
  return (stateHandlerPtr != nullptr && stateHandlerPtr->state() == EntityState::RemovedByThis);
}

void NestedStore::doCommitChanges() {
  if (m_isCommitting) {
    // The purpose of this is to prevent committing from destructor when the NestedStore is destructed because of an
    // exception is thrown from this function.
    return;
  }

  const auto removeWithCheck = [this](const EntityId id) {
    if (!m_parentStore.remove(id)) {
      throw std::logic_error("Cannot remove Entity while committing changes to parent!");
    }
  };

  const auto handleEntity = [this, &removeWithCheck](Entity &entity) {
    const auto &stateHandler = m_statesManager.getState(entity.id());

    if (stateHandler.needsToUpdateInParent()) {
      const auto entityId = entity.id();
      if (m_parentStore.update(entityId, std::move(entity).properties()) == nullptr) {
        throw std::logic_error("Cannot update Entity while committing changes to parent!");
      }
    } else {
      if (stateHandler.needsToRemoveFromParent()) {
        removeWithCheck(entity.id());
      }
      if (stateHandler.needsToInsertToParent()) {
        const auto entityId = entity.id();
        if (!m_parentStore.insert(entityId, std::move(entity).properties())) {
          throw std::logic_error("Cannot insert Entity while committing changes to parent!");
        }
      }
    }
  };

  m_isCommitting = true;
  for (auto &entityHolder: std::move(m_ownStore)) {
    if (entityHolder.has_value()) {
      auto &entity = *entityHolder;
      handleEntity(entity);
      m_statesManager.eraseStateHandler(entity.id());
    }
  }

  for (auto const &[entityId, stateHandler]: m_statesManager.stateHandlers()) {
    if (stateHandler.state() != EntityState::RemovedByThis) {
      throw std::logic_error("Entity is expected to be in RemovedByThis state, but it isn't!");
    }
    removeWithCheck(entityId);
  }
  m_isCommitting = false;
}

void NestedStore::reset() {
  m_ownStore = RootStore();
  m_statesManager = EntityStatesManager();
}

} // namespace EntityStore
