#include "EntityStore/Internal/NestedStore.hpp"

#include <utility>

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

template <SameAsProperties TProperties>
const Properties *doUpdate(const IStore &parentStore, RootStore &ownStore, EntityStatesManager &statesManager,
                           const EntityId id, TProperties &&properties) {
  auto transaction = statesManager.startUpdate(id);
  if (transaction->state() == EntityState::RemovedByThis) {
    transaction.abort();
    return nullptr;
  }

  const auto *updatedPropertiesPtr = ownStore.update(id, std::forward<TProperties>(properties));
  if (updatedPropertiesPtr != nullptr) {
    return updatedPropertiesPtr;
  }

  const auto *parentPropertiesPtr = parentStore.tryGet(id);
  if (parentPropertiesPtr == nullptr) {
    transaction.abort();
    return nullptr;
  }

  ownStore.insert(id, *parentPropertiesPtr);
  return ownStore.update(id, std::forward<TProperties>(properties));
}

NestedStore::NestedStore(IStore &parentStore)
  : m_parentStore{&parentStore}
  , m_ownStore{}
  , m_statesManager{} {
}

NestedStore::NestedStore(NestedStore &&other)
  : m_parentStore{other.m_parentStore.get()}
  , m_ownStore{std::move(other.m_ownStore)}
  , m_statesManager{std::move(other.m_statesManager)} {
  other.m_parentStore = nullptr;
}

NestedStore &NestedStore::operator=(NestedStore &&other) {
  if (&other != this) {
    m_parentStore = std::exchange(other.m_parentStore, nullptr);
    m_ownStore = std::move(other.m_ownStore);
    m_statesManager = std::move(other.m_statesManager);
  }
  return *this;
}

bool NestedStore::insert(const EntityId id, Properties &&properties) {
  return doInsert(*m_parentStore, m_ownStore, m_statesManager, id, std::move(properties));
}

bool NestedStore::insert(const EntityId id, const Properties &properties) {
  return doInsert(*m_parentStore, m_ownStore, m_statesManager, id, properties);
}

const Properties *NestedStore::update(const EntityId id, Properties &&properties) {
  return doUpdate(*m_parentStore, m_ownStore, m_statesManager, id, std::move(properties));
}

// This function differs from the && version, see the comments:
const Properties *NestedStore::update(const EntityId id, const Properties &properties) {
  return doUpdate(*m_parentStore, m_ownStore, m_statesManager, id, properties);
}

bool NestedStore::contains(const EntityId id) const {
  return m_ownStore.contains(id) || (!isRemovedByThisChild(id) && m_parentStore->contains(id));
}

const Properties *NestedStore::tryGet(const EntityId id) const {
  const auto *propertiesPtr = m_ownStore.tryGet(id);
  if (propertiesPtr != nullptr) {
    return propertiesPtr;
  }

  if (isRemovedByThisChild(id)) {
    return nullptr;
  }
  return m_parentStore->tryGet(id);
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
    isRemoveSuccessfull = m_parentStore->contains(id);
  }

  if (!isRemoveSuccessfull) {
    transaction.abort();
  }

  return isRemoveSuccessfull;
}

std::unordered_set<EntityId> NestedStore::filterIds(const EntityPredicate &predicate) const {
  std::unordered_set<EntityId> result;
  result = m_ownStore.filterIds(predicate);
  result.merge(m_parentStore->filterIds(IgnoreIds{m_statesManager.createEntityIdsSet()} | predicate));
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
  const auto removeWithCheck = [this](const EntityId id) {
    if (!m_parentStore->remove(id)) {
      throw std::logic_error("Cannot remove Entity while committing changes to parent!");
    }
  };

  const auto handleEntity = [this, &removeWithCheck](Entity &&entity) {
    const auto &stateHandler = m_statesManager.getState(entity.id());

    if (stateHandler.needsToUpdateInParent()) {
      const auto entityId = entity.id();
      if (m_parentStore->update(entityId, std::move(entity).properties()) == nullptr) {
        throw std::logic_error("Cannot update Entity while committing changes to parent!");
      }
    } else {
      if (stateHandler.needsToRemoveFromParent()) {
        removeWithCheck(entity.id());
      }
      if (stateHandler.needsToInsertToParent()) {
        const auto entityId = entity.id();
        if (!m_parentStore->insert(entityId, std::move(entity).properties())) {
          throw std::logic_error("Cannot insert Entity while committing changes to parent!");
        }
      }
    }
  };

  for (auto &&entityHolder: std::move(m_ownStore)) {
    if (entityHolder.has_value()) {
      auto entityId = entityHolder->id();
      handleEntity(std::move(*entityHolder));
      m_statesManager.eraseStateHandler(entityId);
    }
  }

  for (auto const &[entityId, stateHandler]: m_statesManager.stateHandlers()) {
    if (stateHandler.state() != EntityState::RemovedByThis) {
      throw std::logic_error("Entity is expected to be in RemovedByThis state, but it isn't!");
    }
    removeWithCheck(entityId);
  }
}

void NestedStore::reset() {
  m_ownStore = RootStore();
  m_statesManager = EntityStatesManager();
}

} // namespace EntityStore
