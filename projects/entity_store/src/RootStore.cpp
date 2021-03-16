#include "RootStore.hpp"

#include <cassert>

#include "StoreExceptions.hpp"

namespace EntityStore {

bool RootStore::insert(const TodoId id, Properties &&properties) {
  return doInsert(id, std::move(properties));
}

bool RootStore::insert(const TodoId id, const Properties &properties) {
  return doInsert(id, properties);
}

const Properties *RootStore::update(const TodoId id, Properties &&properties) {
  return doUpdate(id, std::move(properties));
}

const Properties *RootStore::update(const TodoId id, const Properties &properties) {
  return doUpdate(id, properties);
}

bool RootStore::contains(const TodoId id) const {
  return m_todoIndexById.find(id) != m_todoIndexById.end();
}

const Properties *RootStore::tryGet(const TodoId id) const {
  auto it = m_todoIndexById.find(id);
  if (it == m_todoIndexById.end()) {
    return nullptr;
  }
  return &m_todos[it->second]->properties;
}

const Properties &RootStore::get(const TodoId id) const {
  auto propertiesPtr = tryGet(id);
  if (propertiesPtr == nullptr) {
    throw DoesNotHaveTodoException(id);
  }
  return *propertiesPtr;
}

bool RootStore::remove(const TodoId id) {
  auto it = m_todoIndexById.find(id);
  if (it == m_todoIndexById.end()) {
    return false;
  }

  auto index = it->second;
  m_todoIndexById.erase(it);
  m_todos[index] = std::nullopt;
  m_emptyIndices.insert(index);

  return true;
}

std::unordered_set<TodoId> RootStore::filterIds(const TodoPredicate &predicate) const {
  std::unordered_set<TodoId> result;

  for (const auto &todoHolder: m_todos) {
    if (!todoHolder.has_value()) {
      continue;
    }
    const auto &todo = *todoHolder;
    if (predicate(todo.id(), todo.properties)) {
      result.insert(todo.id());
    }
  }
  return result;
}

void RootStore::commit() {
}

void RootStore::rollback() {
}

void RootStore::shrink() {

  if (m_todos.size() == m_todoIndexById.size()) {
    return;
  }

  if (m_todoIndexById.empty()) {
    m_todos.clear();
    m_todos.shrink_to_fit();
    return;
  }

  if constexpr (std::is_nothrow_swappable_v<TodoVector::value_type>) {
    auto shrinkWithMove = [&]() {
      auto updateToNextNotEmpty = [this](TodoVector::reverse_iterator &it) {
        while (it != m_todos.rend() && !it->has_value()) {
          ++it;
        }
      };

      auto updateToNextEmpty = [this](TodoVector::iterator &it) {
        while (it != m_todos.end() && it->has_value()) {
          ++it;
        }
      };

      auto getIndexByIt = [this](TodoVector::iterator &it) { return static_cast<size_t>(it - m_todos.begin()); };

      auto emptyHolderIt = m_todos.begin();
      updateToNextEmpty(emptyHolderIt);
      auto notEmptyHolderIt = m_todos.rbegin();
      updateToNextNotEmpty(notEmptyHolderIt);
      while (emptyHolderIt < notEmptyHolderIt.base()) {
        auto newIndex = getIndexByIt(emptyHolderIt);

        m_todoIndexById.at((*notEmptyHolderIt)->id()) = newIndex;
        std::swap(*emptyHolderIt, *notEmptyHolderIt);
        updateToNextEmpty(emptyHolderIt);
        updateToNextNotEmpty(notEmptyHolderIt);
      }
      m_todos.erase(emptyHolderIt, m_todos.end());
      m_todos.shrink_to_fit();
    };
    shrinkWithMove();
  } else {
    static_assert(std::is_nothrow_swappable_v<TodoVector>, "Possible dataloss if the TodoVector::swap throws!");
    static_assert(std::is_nothrow_swappable_v<IdToIndexMap>, "Possible dataloos if the IdToIndexMap::swap throws!");
    auto shrinkWithCopy = [&] {
      TodoVector newTodos;
      newTodos.reserve(m_todoIndexById.size());
      IdToIndexMap newTodoIndexById;
      for (const auto &todoHolder: m_todos) {
        if (todoHolder.has_value()) {
          newTodoIndexById.emplace(todoHolder->id(), newTodos.size());
          newTodos.push_back(*todoHolder);
        }
      }
      m_todos.swap(newTodos);
      m_todoIndexById.swap(newTodoIndexById);
    };
    shrinkWithCopy();
  }
  assert(m_todos.size() == m_todoIndexById.size());
  m_emptyIndices.clear();
}

void RootStore::doInsert(Todo &&todo) {
  auto insertTodo = [this](Todo &&todo) {
    size_t usedIndex = m_todos.size();
    m_todoIndexById.emplace(todo.id(), usedIndex);
    m_todos.push_back(std::move(todo));
  };

  auto assignTodoToEmptyIndex = [this](Todo &&todo) {
    auto firstEmptyIndex = m_emptyIndices.begin();
    size_t usedIndex = *firstEmptyIndex;
    m_emptyIndices.erase(firstEmptyIndex);
    m_todoIndexById.emplace(todo.id(), usedIndex);

    m_todos[usedIndex] = std::move(todo);
  };

  if (m_emptyIndices.empty()) {
    insertTodo(std::move(todo));
  } else {
    assignTodoToEmptyIndex(std::move(todo));
  }
}

} // namespace EntityStore
