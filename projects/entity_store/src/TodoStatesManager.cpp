#include "TodoStatesManager.hpp"

namespace EntityStore {

void TodoStateHandler::insert() {
  switch (m_state) {
  case TodoState::Default:
    m_state = TodoState::InsertedByThis;
    return;
  case TodoState::RemovedByThis:
    m_state = TodoState::ReInsertedByThis;
    return;
  default:
    throw std::logic_error("Invalid state for insert!");
  }
}

void TodoStateHandler::update() {
  switch (m_state) {
  case TodoState::Default:
    m_state = TodoState::OnlyUpdated;
    return;
  case TodoState::OnlyUpdated:
  case TodoState::InsertedByThis:
  case TodoState::ReInsertedByThis:
    // the state intentionally not changed
    return;
  default:
    throw std::logic_error("Invalid state for insert!");
  }
}

void TodoStateHandler::remove() {
  switch (m_state) {
  case TodoState::Default:
  case TodoState::OnlyUpdated:
    m_state = TodoState::RemovedByThis;
    return;
  case TodoState::InsertedByThis:
    m_state = TodoState::Default;
    return;
  case TodoState::ReInsertedByThis:
    m_state = TodoState::RemovedByThis;
    return;
  default:
    throw std::logic_error("Invalid state for remove!");
  }
}

bool TodoStateHandler::needsToInsertToParent() const {
  return m_state == TodoState::InsertedByThis || m_state == TodoState::ReInsertedByThis;
}

bool TodoStateHandler::needsToUpdateInParent() const {
  return m_state == TodoState::OnlyUpdated;
}

bool TodoStateHandler::needsToRemoveFromParent() const {
  return m_state == TodoState::RemovedByThis || m_state == TodoState::ReInsertedByThis;
}

const TodoState &TodoStateHandler::state() const {
  return m_state;
}

const TodoStateHandler *TodoStatesManager::tryGetState(const TodoId id) const {
  auto it = m_stateHandlers.find(id);
  if (it == m_stateHandlers.end()) {
    return nullptr;
  }
  return &it->second;
}

const TodoStateHandler &TodoStatesManager::getState(const TodoId id) const {
  return m_stateHandlers.at(id);
}

TodoTransaction TodoStatesManager::startInsert(const TodoId id) {
  return TodoTransaction(*this, id, TodoTransaction::Type::Insert);
}

TodoTransaction TodoStatesManager::startUpdate(const TodoId id) {
  return TodoTransaction(*this, id, TodoTransaction::Type::Update);
}

TodoTransaction TodoStatesManager::startRemove(const TodoId id) {
  return TodoTransaction(*this, id, TodoTransaction::Type::Remove);
}

std::unordered_set<TodoId> TodoStatesManager::createTodoIdsSet() const {
  std::unordered_set<TodoId> todoIds;
  todoIds.reserve(m_stateHandlers.size());
  for (const auto &p: m_stateHandlers) {
    todoIds.insert(p.first);
  }
  return todoIds;
}

bool TodoStatesManager::eraseStateHandler(const TodoId id) {
  return m_stateHandlers.erase(id) > 0;
}

const TodoStatesManager::StateHandlerMap &TodoStatesManager::stateHandlers() const {
  return m_stateHandlers;
}

void TodoTransaction::abort() {
  m_aborted = true;
}

TodoStateHandler &TodoTransaction::operator*() {
  return m_handlerIt->second;
}

const TodoStateHandler &TodoTransaction::operator*() const {
  return m_handlerIt->second;
}

TodoStateHandler *TodoTransaction::operator->() {
  return &m_handlerIt->second;
}

const TodoStateHandler *TodoTransaction::operator->() const {
  return &m_handlerIt->second;
}

TodoTransaction::TodoTransaction(TodoStatesManager &manager, const TodoId id, Type type)
  : m_manager{manager}
  , m_type{type}
  , m_handlerIt{}
  , m_aborted{false} {
  m_handlerIt = m_manager.m_stateHandlers.try_emplace(id, TodoStateHandler()).first;
}

TodoTransaction::~TodoTransaction() {
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
    if (m_handlerIt->second.state() == TodoState::Default) {
      m_handlerIt = m_manager.m_stateHandlers.erase(m_handlerIt);
    }
  } catch (...) {
    // logging something is a better idea then doing nothing
  }
}

} // namespace EntityStore
