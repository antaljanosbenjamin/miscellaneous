#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "IStore.hpp"
#include "Properties.hpp"
#include "Todo.hpp"
#include "TodoPredicate.hpp"

namespace EntityStore {

class NestedStore;

// This is a basic implementation for IStore: it just implements the absolute necessary functionality to be able to
// behave as an IStore.

class RootStore : public IStore {
public:
  friend NestedStore;

  RootStore() = default;
  RootStore(const RootStore &) = default;
  RootStore(RootStore &&) = default;
  RootStore &operator=(const RootStore &) = default;
  RootStore &operator=(RootStore &&) = default;
  ~RootStore() override = default;

  bool insert(const TodoId id, Properties &&properties) override;
  bool insert(const TodoId id, const Properties &properties) override;

  const Properties *update(const TodoId id, Properties &&properties) override;
  const Properties *update(const TodoId id, const Properties &properties) override;

  bool contains(const TodoId id) const override;
  const Properties *tryGet(const TodoId id) const override;
  const Properties &get(const TodoId id) const override;

  bool remove(const TodoId id) override;

  std::unordered_set<TodoId> filterIds(const TodoPredicate &predicate) const override;

  void commit() override;
  void rollback() override;

  void shrink() override;

private:
  template <class TProperties, class = std::enable_if_t<std::is_same_v<std::decay_t<TProperties>, Properties>>>
  bool doInsert(const TodoId id, TProperties &&properties) {
    auto it = m_todoIndexById.find(id);
    if (it != m_todoIndexById.end()) {
      return false;
    };
    doInsert(Todo{id, std::forward<TProperties>(properties)});
    return true;
  }
  void doInsert(Todo &&todo);

  template <class TProperties, class = std::enable_if_t<std::is_same_v<std::decay_t<TProperties>, Properties>>>
  const Properties *doUpdate(const TodoId id, TProperties &&properties) {
    auto it = m_todoIndexById.find(id);
    if (it == m_todoIndexById.end()) {
      return nullptr;
    }
    auto &todoHolder = m_todos[it->second];
    todoHolder->update(std::forward<TProperties>(properties));
    return &todoHolder->properties;
  }

  using TodoVector = std::vector<std::optional<Todo>>;
  using IdToIndexMap = std::unordered_map<TodoId, size_t>;

  // Using a map as index on top of a vector is almost forge the best of the two:
  //  * The map provides O(1) lookup time in best scenario, and O(n) in the worst. The worst means every TodoId in the
  //  map has the same hash value. If this causes any problem, then something is wrong with the hash function.
  //  * The vector provides fast iterating for queries, and it is more cache friendly then a map.

  TodoVector m_todos;
  IdToIndexMap m_todoIndexById;

  // By keeping the empty indices sorted and using always its first element helps to concentrate the Todos closer in the
  // memory, which might provide some performance benefits. To say more some measurement should be done based on the
  // usage profile.
  std::set<size_t> m_emptyIndices;
};

} // namespace EntityStore
