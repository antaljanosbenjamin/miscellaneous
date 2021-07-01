#pragma once

#include <unordered_set>

#include "EntityStore/Internal/Entity.hpp"
#include "EntityStore/Internal/EntityPredicate.hpp"
#include "EntityStore/Internal/EntityStatesManager.hpp"
#include "EntityStore/Internal/IStore.hpp"
#include "EntityStore/Internal/RootStore.hpp"
#include "EntityStore/Properties.hpp"
#include "utils/PropagateConst.hpp"

namespace EntityStore {

// This class contains the logic that necessary to have nested stores. It knows about and uses very frequently the
// parent store. It also maintain the state of the Entities that are inserted/modified/removed through it.

class NestedStore : public IStore {
public:
  explicit NestedStore(IStore &parentStore);
  ~NestedStore() override = default;

  // For NestedStore, these operations are ambiguous:
  //  * Default Ctor: what will be the parent store?
  //  * Copy Ctor: what is the purpose of copying a NestedStore? The only use-case which came into my mind is to create
  //  restore points within a long transaction, but this is supported by nesting an other NestedStore. Apart from that,
  //  copying a store will almost lead to conflicting transactions, so it is better not to provide this functionality.
  //  * Move Ctor: Although, moving a NestedStore can make sense, but the meaning of moved-from NestedStore object is
  //  ambiguous: it must be in a valid, but unspecified state. As currently there is no need to move a NestedStore, I
  //  decided to delete the Move Ctor also.
  //  * operator=: the same reasons as above.
  NestedStore() = delete;
  NestedStore(const NestedStore &) = delete;
  NestedStore(NestedStore &&other);
  NestedStore &operator=(const NestedStore &) = delete;
  NestedStore &operator=(NestedStore &&other);

  bool insert(const EntityId id, Properties &&properties) override;
  bool insert(const EntityId id, const Properties &properties) override;

  const Properties *update(const EntityId id, Properties &&properties) override;
  const Properties *update(const EntityId id, const Properties &properties) override;

  bool contains(const EntityId id) const override;
  const Properties *tryGet(const EntityId id) const override;
  const Properties &get(const EntityId id) const override;

  bool remove(const EntityId id) override;

  std::unordered_set<EntityId> filterIds(const EntityPredicate &predicate) const override;

  void commit() override;
  void rollback() override;

  void shrink() override;

private:
  bool isRemovedByThisChild(const EntityId id) const;

  void doCommitChanges();
  void reset();

  utils::PropagateConst<IStore *> m_parentStore;
  RootStore m_ownStore;
  EntityStatesManager m_statesManager;
};

} // namespace EntityStore
