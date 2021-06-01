#pragma once

#include <unordered_set>

#include "IStore.hpp"
#include "Properties.hpp"
#include "RootStore.hpp"
#include "Todo.hpp"
#include "TodoPredicate.hpp"
#include "TodoStatesManager.hpp"

namespace EntityStore {

// This class contains the logic that necessary to have nested stores. It knows about and uses very frequently the
// parent store. It also maintain the state of the Todos that are inserted/modified/removed through it.

// It doesn't inherit from RootStore, but it would like to access it's members for better performance. The main reason
// why it doesn't inherit from it is "a NestedStore is not a RootStore", it is a different IStore implementation.

class NestedStore : public IStore {
public:
  explicit NestedStore(IStore &parentStore);
  ~NestedStore() override;

  // For NestedStore, these operations are ambiguous:
  //  * Default Ctor: what will be the parent store?
  //  * Copy Ctor: what is the purpose of copying a NestedStore? The only use-case which came into my mind is to create
  //  restore points within a long transaction, but this is supported by nesting an other NestedStore. Apart from that,
  //  copying a store will almost lead to conflicting transactions, so it is better to not provide this functionality.
  //  * Move Ctor: Although, moving a NestedStore can make sense, but the meaning of moved-from NestedStore object is
  //  ambiguous: it must be in a valid, but unspecified state. That means, the moved-from object must be able to handle
  //  operations, which can also easily lead to conflicts. As currently there is no need to move a NestedStore, I
  //  decided to delete the Move Ctor also.
  //  * operator=: the same reasons as above.
  NestedStore() = delete;
  NestedStore(const NestedStore &) = delete;
  NestedStore(NestedStore &&) = delete;
  NestedStore &operator=(const NestedStore &) = delete;
  NestedStore &operator=(NestedStore &&) = delete;

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
  bool isRemovedByThisChild(const TodoId id) const;

  void doCommitChanges();
  void doRollback();

  template <class TProperties, class = std::enable_if_t<std::is_same_v<std::decay_t<TProperties>, Properties>>>
  bool doInsert(const TodoId id, TProperties &&properties) {
    auto transaction = m_statesManager.startInsert(id);

    if (transaction->state() == TodoState::RemovedByThis) {
      m_ownStore.insert(id, std::forward<TProperties>(properties));
      return true;
    }
    if (m_parentStore.contains(id)) {
      transaction.abort();
      return false;
    }
    auto isOwnInsertSuccessfull = m_ownStore.insert(id, std::forward<TProperties>(properties));
    if (!isOwnInsertSuccessfull) {
      transaction.abort();
    }
    return isOwnInsertSuccessfull;
  }

  IStore &m_parentStore;
  RootStore m_ownStore;
  TodoStatesManager m_statesManager;
  bool m_isCommitting;
};

} // namespace EntityStore
