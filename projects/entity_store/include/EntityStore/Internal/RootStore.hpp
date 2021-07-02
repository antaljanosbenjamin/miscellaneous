#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "EntityStore/Internal/Entity.hpp"
#include "EntityStore/Internal/EntityPredicate.hpp"
#include "EntityStore/Internal/IStore.hpp"
#include "EntityStore/Properties.hpp"

namespace EntityStore {

// This is a basic implementation for IStore: it just implements the absolute necessary functionality to be able to
// behave as an IStore.

class RootStore : public IStore {
private:
  static_assert((sizeof(Entity) + sizeof(void *)) == sizeof(std::optional<Entity>),
                "The overhead of optional is too big!");
  using EntityVector = std::vector<std::optional<Entity>>;

public:
  using Iterator = EntityVector::iterator;
  using ConstIterator = EntityVector::const_iterator;

  RootStore() = default;
  RootStore(const RootStore &) = default;
  RootStore(RootStore &&) = default;
  RootStore &operator=(const RootStore &) = default;
  RootStore &operator=(RootStore &&) = default;
  ~RootStore() override = default;

  bool insert(const EntityId id, Properties &&properties) override;
  bool insert(const EntityId id, const Properties &properties) override;

  const Properties *update(const EntityId id, Properties &&properties) override;
  const Properties *update(const EntityId id, const Properties &properties) override;

  [[nodiscard]] bool contains(const EntityId id) const override;
  [[nodiscard]] const Properties *tryGet(const EntityId id) const override;
  [[nodiscard]] const Properties &get(const EntityId id) const override;

  bool remove(const EntityId id) override;

  std::unordered_set<EntityId> filterIds(const EntityPredicate &predicate) const override;

  void commit() override;
  void rollback() override;
  void shrink() override;

  [[nodiscard]] Iterator begin();
  [[nodiscard]] Iterator end();
  [[nodiscard]] ConstIterator begin() const;
  [[nodiscard]] ConstIterator end() const;

private:
  using IdToIndexMap = std::unordered_map<EntityId, size_t>;

  // Using a map as index on top of a vector is almost forge the best of the two:
  //  * The map provides O(1) lookup time in best scenario, and O(n) in the worst. The worst means every EntityId in
  //  the map has the same hash value. If this causes any problem, then something is wrong with the hash function.
  //  * The vector provides fast iteration for queries, and it is more cache friendly then a map.

  EntityVector m_entities;
  IdToIndexMap m_entityIndexById;

  // By keeping the empty indices sorted and using always its first element helps to concentrate the Entities closer in
  // the memory, which might provide some performance benefits. To say more some measurement should be done.
  std::set<size_t> m_emptyIndices;
};

} // namespace EntityStore
