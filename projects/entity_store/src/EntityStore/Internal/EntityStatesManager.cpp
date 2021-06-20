#include "EntityStore/Internal/EntityStatesManager.hpp"

namespace EntityStore {

void EntityStateHandler::insert() {
  switch (m_state) {
  case EntityState::Default:
    m_state = EntityState::InsertedByThis;
    return;
  case EntityState::RemovedByThis:
    m_state = EntityState::ReInsertedByThis;
    return;
  case EntityState::InsertedByThis:
  case EntityState::OnlyUpdated:
  case EntityState::ReInsertedByThis:
    throw std::logic_error("Invalid state for insert!");
  }
}

void EntityStateHandler::update() {
  switch (m_state) {
  case EntityState::Default:
    m_state = EntityState::OnlyUpdated;
    return;
  case EntityState::OnlyUpdated:
  case EntityState::InsertedByThis:
  case EntityState::ReInsertedByThis:
    // the state intentionally not changed
    return;
  case EntityState::RemovedByThis:
    throw std::logic_error("Invalid state for insert!");
  }
}

void EntityStateHandler::remove() {
  switch (m_state) {
  case EntityState::Default:
  case EntityState::OnlyUpdated:
    m_state = EntityState::RemovedByThis;
    return;
  case EntityState::InsertedByThis:
    m_state = EntityState::Default;
    return;
  case EntityState::ReInsertedByThis:
    m_state = EntityState::RemovedByThis;
    return;
  case EntityState::RemovedByThis:
    throw std::logic_error("Invalid state for remove!");
  }
}

bool EntityStateHandler::needsToInsertToParent() const {
  // If it is reinserted, then the new entity might not contain some of the original properties, therefore the previous
  // properties have to be removed.
  return m_state == EntityState::InsertedByThis || m_state == EntityState::ReInsertedByThis;
}

bool EntityStateHandler::needsToUpdateInParent() const {
  return m_state == EntityState::OnlyUpdated;
}

bool EntityStateHandler::needsToRemoveFromParent() const {
  return m_state == EntityState::RemovedByThis || m_state == EntityState::ReInsertedByThis;
}

const EntityState &EntityStateHandler::state() const {
  return m_state;
}

const EntityStateHandler *EntityStatesManager::tryGetState(const EntityId id) const {
  auto it = m_stateHandlers.find(id);
  if (it == m_stateHandlers.end()) {
    return nullptr;
  }
  return &it->second;
}

const EntityStateHandler &EntityStatesManager::getState(const EntityId id) const {
  return m_stateHandlers.at(id);
}

EntityTransaction EntityStatesManager::startInsert(const EntityId id) {
  return EntityTransaction(*this, id, EntityTransaction::Type::Insert);
}

EntityTransaction EntityStatesManager::startUpdate(const EntityId id) {
  return EntityTransaction(*this, id, EntityTransaction::Type::Update);
}

EntityTransaction EntityStatesManager::startRemove(const EntityId id) {
  return EntityTransaction(*this, id, EntityTransaction::Type::Remove);
}

std::unordered_set<EntityId> EntityStatesManager::createEntityIdsSet() const {
  std::unordered_set<EntityId> entityIds;
  entityIds.reserve(m_stateHandlers.size());
  for (const auto &p: m_stateHandlers) {
    entityIds.insert(p.first);
  }
  return entityIds;
}

bool EntityStatesManager::eraseStateHandler(const EntityId id) {
  return m_stateHandlers.erase(id) > 0;
}

const EntityStatesManager::StateHandlerMap &EntityStatesManager::stateHandlers() const {
  return m_stateHandlers;
}

void EntityTransaction::abort() {
  m_aborted = true;
}

EntityStateHandler &EntityTransaction::operator*() {
  return m_handlerIt->second;
}

const EntityStateHandler &EntityTransaction::operator*() const {
  return m_handlerIt->second;
}

EntityStateHandler *EntityTransaction::operator->() {
  return &m_handlerIt->second;
}

const EntityStateHandler *EntityTransaction::operator->() const {
  return &m_handlerIt->second;
}

EntityTransaction::EntityTransaction(EntityStatesManager &manager, const EntityId id, Type type)
  : m_manager{manager}
  , m_type{type}
  , m_handlerIt{m_manager.m_stateHandlers.try_emplace(id, EntityStateHandler()).first}
  , m_aborted{false} {
}

EntityTransaction::~EntityTransaction() {
  try {
    if (!m_aborted) {
      switch (m_type) {
      case Type::Insert:
        m_handlerIt->second.insert();
        break;
      case Type::Update:
        m_handlerIt->second.update();
        break;
      case Type::Remove:
        m_handlerIt->second.remove();
        break;
      }
    }
    if (m_handlerIt->second.state() == EntityState::Default) {
      m_handlerIt = m_manager.m_stateHandlers.erase(m_handlerIt);
    }
  } catch (...) {
    // logging something is a better idea then doing nothing
  }
}

} // namespace EntityStore
